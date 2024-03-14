# pg_remote_exec

PostgreSQL extension to allow shell command execution via SQL query.

# INSTALL

Install PostgreSQL before proceeding. Make sure to have `pg_config` binary,
these are typically included in `-dev` and `-devel` packages.

```bash
git clone https://github.com/cybertec-postgresql/pg_remote_exec.git
cd pg_remote_exec
make
sudo make install
```

# CONFIGURE

Execute as superuser:

```
CREATE EXTENSION pg_remote_exec;
```

# USAGE

```
postgres=# select pg_remote_exec('date');
 pg_remote_exec
----------------
              0
(1 row)

postgres=# select pg_remote_exec_fetch('date', 't');
     pg_remote_exec_fetch
───────────────────────────────
 Fri Aug  5 17:41:07 EEST 2016
(1 row)
```

# REFERENCE

## Functions

* `pg_remote_exec(text)`: executes the command and returns the shell
  exit code.
* `pg_remote_exec_fetch(text, boolean)`: executes the command and returns
  output as text. The boolean parameter dictates whether to return output in
  case of non-zero exit code.
