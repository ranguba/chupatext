SUBDIRS =
EMPTY =

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =			\
	test_metadata.la \
	test_text.la \
	test_text_input_stream.la \
	test_decomposer.la \
	test_restrict_input_stream.la \
	$(EMPTY)

if HAVE_POPPLER_GLIB
noinst_LTLIBRARIES += test_pdf.la
endif

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
	$(GCUTTER_CFLAGS) \
	$(GLIB_CFLAGS) \
	$(EMPTY)

AM_LDFLAGS =			\
	-module			\
	-rpath "`pwd`/.libs"	\
	-avoid-version		\
	-no-undefined

LIBS =					\
	$(top_builddir)/chupatext/libchupatext.la \
	$(GCUTTER_LIBS) \
	$(GLIB_LIBS) \
	$(EMPTY)

test_metadata_la_SOURCES = test_metadata.c
test_text_la_SOURCES = test_text.c
test_text_input_stream_la_SOURCES = test_text_input_stream.c
test_decomposer_la_SOURCES = test_decomposer.c
test_restrict_input_stream_la_SOURCES = test_restrict_input_stream.c
test_pdf_la_SOURCES = test_pdf.c

echo-cutter:
	@echo $(CUTTER)
