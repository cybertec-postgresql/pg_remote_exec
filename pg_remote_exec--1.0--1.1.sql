-- We do not want anyone but superuser to be able to invoke those.
REVOKE ALL ON FUNCTION pg_remote_exec() FROM PUBLIC;
REVOKE ALL ON FUNCTION pg_remote_exec_fetch() FROM PUBLIC;
