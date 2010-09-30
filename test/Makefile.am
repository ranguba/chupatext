SUBDIRS = fixtures
EMPTY =

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

noinst_LTLIBRARIES =				\
	suite_chupatext_test.la			\
	test_metadata.la			\
	test_text.la				\
	test_text_input.la			\
	test_text_input_stream.la		\
	test_decomposer.la			\
	test_tar.la				\
	test_zip.la				\
	test_ruby.la				\
	$(EMPTY)

if HAVE_POPPLER_GLIB
noinst_LTLIBRARIES += test_pdf.la
endif

if HAVE_WV
noinst_LTLIBRARIES += test_word.la
endif
if HAVE_GOFFICE
noinst_LTLIBRARIES += test_excel.la
endif

noinst_LTLIBRARIES += test_powerpoint.la
endif

EXTRA_DIST =					\
	run-test.sh				\
	chupa_test_util.h

INCLUDES =		\
	-I$(srcdir)	\
	-I$(top_srcdir) \
	-I$(top_includedir) \
	-I$(build_includedir)

AM_CFLAGS = 				\
	$(CHUPATEXT_CFLAGS) \
	$(GCUTTER_CFLAGS) \
	$(GSF_CFLAGS) \
	$(EMPTY)

AM_LDFLAGS =			\
	-module			\
	-rpath "`pwd`/.libs"	\
	-avoid-version		\
	-no-undefined

LIBS =					\
	$(top_builddir)/chupatext/libchupatext.la \
	$(GCUTTER_LIBS) \
	$(GSF_LIBS) \
	$(EMPTY)

suite_chupatext_test_la_SOURCES = suite_chupatext_test.c

test_metadata_la_SOURCES = test_metadata.c
test_text_la_SOURCES = test_text.c chupa_test_util.c
test_text_input_la_SOURCES = test_text_input.c
test_text_input_stream_la_SOURCES = test_text_input_stream.c
test_decomposer_la_SOURCES = test_decomposer.c
test_pdf_la_SOURCES = test_pdf.c chupa_test_util.c
test_word_la_SOURCES = test_word.c chupa_test_util.c
test_excel_la_SOURCES = test_excel.c chupa_test_util.c
test_powerpoint_la_SOURCES = test_powerpoint.c chupa_test_util.c
test_tar_la_SOURCES = test_tar.c chupa_test_util.c
test_zip_la_SOURCES = test_zip.c chupa_test_util.c
test_ruby_la_SOURCES = test_ruby.c chupa_test_util.c

echo-cutter:
	@echo $(CUTTER)
