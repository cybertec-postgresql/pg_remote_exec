CREATE EXTENSION IF NOT EXISTS pg_remote_exec;

select pg_remote_exec('date');

select pg_remote_exec_fetch('echo "test"', 't');

select pg_remote_exec_fetch('this_cmd_does_not_exist', 'f');
