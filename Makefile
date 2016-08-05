# contrib/remote_exec/Makefile

MODULES = pg_remote_exec

EXTENSION = pg_remote_exec
DATA = pg_remote_exec--1.0.sql
PGFILEDESC = "pg_remote_exec - remote execution"

REGRESS = pg_remote_exec

ifdef USE_PGXS
PG_CONFIG = pg_config
PGXS := $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
else
subdir = contrib/pg_remote_exec
top_builddir = ../..
include $(top_builddir)/src/Makefile.global
include $(top_srcdir)/contrib/contrib-global.mk
endif
