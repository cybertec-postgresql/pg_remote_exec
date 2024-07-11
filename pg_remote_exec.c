#include "postgres.h"

#include "catalog/pg_authid.h"
#include "funcapi.h"
#include "miscadmin.h"
#include "utils/acl.h"
#include "utils/builtins.h"
#include "utils/elog.h"

PG_MODULE_MAGIC;

#if PG_VERSION_NUM < 120000
#error "Unsupported PostgreSQL Version"
#endif

typedef struct OutputContext
{
	FILE   *fp;
	char   *line;
	size_t  len;
} OutputContext;

/* Verify the user has sufficent privileges. */
static inline void check_privileges(void);

/* Execute a given shell command and return status. */
PG_FUNCTION_INFO_V1(pg_remote_exec);
/* Execute a given shell command and return its stdout as a string. */
PG_FUNCTION_INFO_V1(pg_remote_exec_fetch);

inline void
check_privileges(void)
{
	if (!has_privs_of_role(GetUserId(),
#if PG_VERSION_NUM < 140000
				DEFAULT_ROLE_EXECUTE_SERVER_PROGRAM
#else
				ROLE_PG_EXECUTE_SERVER_PROGRAM
#endif
				))
		ereport(ERROR,
				errcode(ERRCODE_INSUFFICIENT_PRIVILEGE),
				errmsg("insufficient privileges"),
				errhint("Only superusers"
					" and members of pg_execute_server_program"
					" may execute this function."));
}

Datum
pg_remote_exec(PG_FUNCTION_ARGS)
{
	int   result;
	char *exec_string;

	check_privileges();

	exec_string = text_to_cstring(PG_GETARG_TEXT_PP(0));
	result = system(exec_string);

	pfree(exec_string);
	PG_RETURN_INT32(result);
}

Datum
pg_remote_exec_fetch(PG_FUNCTION_ARGS)
{
	FuncCallContext *funcctx;
	OutputContext   *ocxt;
	ssize_t          read;
	text            *result;
	bool             ignore_errors;

	check_privileges();

	ignore_errors = PG_GETARG_BOOL(1);

	if (SRF_IS_FIRSTCALL())
	{
		char          *exec_string;
		MemoryContext  oldcontext;

		/*
		 * This chunk will eventually be freed by PG executor. I'm not sure if
		 * it's wise to free it immediately after the popen() call - might
		 * libc still need it during output retrieval? In any case, we don't
		 * need to access the chunk anymore, so it's ok to define it as a
		 * local variable here.
		 */
		exec_string = text_to_cstring(PG_GETARG_TEXT_PP(0));

		funcctx = SRF_FIRSTCALL_INIT();

		/*
		 * palloc0() (unlike palloc()) sets the allocated chung to all zeroes,
		 * so we don't need to explicitly set ocxt->line to NULL, nor
		 * ocxt->len to 0.
		 */
		oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
		ocxt = (OutputContext *) palloc0(sizeof(OutputContext));
		MemoryContextSwitchTo(oldcontext);
		errno = 0;
		ocxt->fp = popen(exec_string, "r");

		if 	(ocxt->fp == NULL)
		{
			if (ignore_errors)
				SRF_RETURN_DONE(funcctx);

			/*
			 * When error occurs, FMGR should free the memory allocated in the
			 * current transaction.
			 */
			ereport(ERROR,
					errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
					errmsg("Failed to run command"));
		}

		/* Make the output context available for the next calls. */
		funcctx->user_fctx = ocxt;
	}

	/*
	 * CHECK_FOR_INTERRUPTS() would make sense here, but I don't know how to
	 * ensure freeing of ocxt->line and ocxt->fp, see comments below.
	 */

	funcctx = SRF_PERCALL_SETUP();
	ocxt = funcctx->user_fctx;

	errno = 0;
	read = getline(&ocxt->line, &ocxt->len, ocxt->fp);
	/* This is serious enough to bring down the whole PG backend. */
	if (errno == EINVAL)
	{
		ereport(FATAL, errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
				errmsg("Failed to read command output."));
	}

	if (read == -1)
	{
		/*
		 * The line buffer was allocated by getline(), so it's not under
		 * control of PG memory management. It's necessary to free it
		 * explicitly.
		 *
		 * The other chunks should be freed by PG executor.
		 */
		if (ocxt->line != NULL)
			free(ocxt->line);

		/* Another resource not controlled by PG. */
		if (pclose(ocxt->fp) != 0 && !ignore_errors)
		{
			ereport(ERROR,
					errcode(ERRCODE_SYNTAX_ERROR_OR_ACCESS_RULE_VIOLATION),
					errmsg("Failed to read command output."));
		}

		SRF_RETURN_DONE(funcctx);
	}

	if (ocxt->line[read - 1] == '\n')
		read -= 1;
	result = cstring_to_text_with_len(ocxt->line, read);

	SRF_RETURN_NEXT(funcctx, PointerGetDatum(result));
}
