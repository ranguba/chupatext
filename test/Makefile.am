SUBDIRS =

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =			\
	test_metadata.la
endif

EXTRA_DIST =		\
	run-test.sh

INCLUDES =		\
	-I$(srcdir)	\
	-I$(top_srcdir) \
	-I$(top_includedir) \
	-I$(build_includedir)

AM_CFLAGS = 				\
	$(CHUPATEXT_CFLAGS) \
	$(GCUTTER_CFLAGS)

AM_LDFLAGS =			\
	-module			\
	-rpath "`pwd`/.libs"	\
	-avoid-version		\
	-no-undefined

LIBS =					\
	$(top_builddir)/chupatext/libchupatext.la \
	$(GCUTTER_LIBS)

test_metadata_la_SOURCES = test_metadata.c

echo-cutter:
	@echo $(CUTTER)
