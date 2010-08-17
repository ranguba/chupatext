SUBDIRS =		\
	lib		\
	fixtures	\
	core		\
	client		\
	libmilter	\
	server		\
	manager		\
	tool

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =			\
	suite-chupatext-test.la
endif

EXTRA_DIST =		\
	run-test.sh

INCLUDES =		\
	-I$(srcdir)	\
	-I$(top_srcdir)

AM_CFLAGS = 				\
	$(CHUPATEXT_CFLAGS)

AM_LDFLAGS =			\
	-module			\
	-rpath "`pwd`/.libs"	\
	-avoid-version		\
	-no-undefined

LIBS =								\
	$(top_builddir)/milter/manager/libchupatext.la

suite_chupatext_test_la_SOURCES = suite-chupatext-test.c

echo-cutter:
	@echo $(CUTTER)
