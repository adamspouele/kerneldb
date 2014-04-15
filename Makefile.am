
AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = -I m4

AM_CPPFLAGS = -I$(srcdir)/src

bin_PROGRAMS = kerneldb

kerneldb_SOURCES = src/main.cpp
kerneldb_LDADD = src/kerneldb/libkerneldb.la

kerneldbdir=$(sysconfdir)/kerneldb/kernels
kerneldb_DATA = \
	kernels/counter.js \
	kernels/executor.js \
	kernels/list.js

SUBDIRS = src

