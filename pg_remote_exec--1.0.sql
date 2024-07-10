-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "CREATE EXTENSION pg_remote_exec" to load this file. \quit

CREATE FUNCTION pg_remote_exec(text)
RETURNS int4
AS 'MODULE_PATHNAME', 'pg_remote_exec'
LANGUAGE C VOLATILE STRICT;

CREATE FUNCTION pg_remote_exec_fetch(text, bool)
RETURNS SETOF text
AS 'MODULE_PATHNAME', 'pg_remote_exec_fetch'
LANGUAGE C VOLATILE STRICT;

REVOKE ALL ON FUNCTION pg_remote_exec() FROM PUBLIC;
REVOKE ALL ON FUNCTION pg_remote_exec_fetch() FROM PUBLIC;
