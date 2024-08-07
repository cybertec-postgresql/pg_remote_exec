MODULE_big = pg_remote_exec
OBJS = pg_remote_exec.o

EXTENSION = pg_remote_exec
DATA = pg_remote_exec--1.0.sql
REGRESS = pg_remote_exec
DOCS = pg_remote_exec.md

PG_CFLAGS = -Wformat

USE_PGXS = 1
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
