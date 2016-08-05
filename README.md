# A PostgreSQL extension to enable execution of shell commands over the normal database connection

# Overview

The extension provides 2 functions (pg_remote_exec, pg_remote_exec_fetch) that
enable execution of user provided input text "as is" on the server shell, under the user used to start the PostgreSQL server.

**Warning!** Users calling the functions need not be "superuser", thus extreme harm could be done very easily. Suitable only for special use cases.

# Functions provided

 * pg_remote_exec(text) - executes the command and returns the shell exit code
 * pg_remote_exec_fetch(text,boolean) - executes the command and returns output as text if any.
 The boolean "ignore_errors" parameter specifies if any output from the command should be returned at all if exit code is nonzero.

# Installation

```
# pg_config needs to be on the path!
# if non-default pg_config location is needed one can specify it similar to USE_PGXS i.e. PG_CONFIG=/path/...
git clone https://github.com/cybertec-postgresql/pg_remote_exec.git
cd pg_remote_exec
USE_PGXS=1 make
sudo USE_PGXS=1 make install

```

# Usage

```
CREATE EXTENSION pg_remote_exec;    -- needs to be done by a Superuser previously

postgres@postgres=# select pg_remote_exec('date');
 pg_remote_exec
────────────────
              0
(1 row)


postgres@postgres=# select pg_remote_exec_fetch('date', 't');
     pg_remote_exec_fetch
───────────────────────────────
 Fri Aug  5 17:41:07 EEST 2016
(1 row)


postgres@postgres=# select pg_remote_exec_fetch('echo "a" && /bin/false', 't');
 pg_remote_exec_fetch
──────────────────────
 a
(1 row)


postgres@postgres=# select pg_remote_exec_fetch('echo "a" && /bin/false', 'f');
ERROR:  Failed to run command
```