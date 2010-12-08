SUBDIRS = fixtures ruby
EMPTY =

if WITH_CUTTER
TESTS = run-test.sh
TESTS_ENVIRONMENT =				\
	NO_MAKE=yes				\
	CUTTER="$(CUTTER)"			\
	RUBY="$(RUBY)"				\
	TOP_BUILD_DIR="$(top_builddir)"		\
	TOP_SRC_DIR="$(top_srcdir)"		\
	TEST_DIR="$(testdir)"

noinst_LTLIBRARIES =				\
	suite_chupatext_test.la			\
	test_metadata.la			\
	test_text.la				\
	test_data.la				\
	test_data_input.la			\
	test_gsf_input_stream.la		\
	test_decomposer.la			\
	test_tar.la				\
	test_gz.la				\
	test_zip.la				\
	test_html.la				\
	test_bin_chupatext.la			\
	test_utils.la				\
	test_mail.la				\
	$(EMPTY)

if SUPPORT_PDF
noinst_LTLIBRARIES += test_pdf.la
endif

if SUPPORT_WORD
noinst_LTLIBRARIES += test_word.la
endif
if SUPPORT_EXCEL
noinst_LTLIBRARIES += test_excel.la
endif

noinst_LTLIBRARIES += test_powerpoint.la
endif

EXTRA_DIST =					\
	run-test.sh				\
	chupa_test_util.h

INCLUDES =					\
	-I$(srcdir)				\
	-I$(top_srcdir)				\
	-I$(top_includedir)			\
	-I$(build_includedir)

AM_CFLAGS =					\
	$(CHUPATEXT_CFLAGS)			\
	$(GCUTTER_CFLAGS)			\
	$(GSF_CFLAGS)				\
	$(EMPTY)

AM_LDFLAGS =			\
	-module			\
	-rpath "`pwd`/.libs"	\
	-avoid-version		\
	-no-undefined

LIBS =							\
	$(top_builddir)/chupatext/libchupatext.la	\
	$(GCUTTER_LIBS)					\
	$(GSF_LIBS)					\
	$(EMPTY)

suite_chupatext_test_la_SOURCES = suite_chupatext_test.c chupa_test_util.c

test_metadata_la_SOURCES = test_metadata.c
test_text_la_SOURCES = test_text.c
test_data_la_SOURCES = test_data.c
test_data_input_la_SOURCES = test_data_input.c
test_gsf_input_stream_la_SOURCES = test_gsf_input_stream.c
test_decomposer_la_SOURCES = test_decomposer.c
test_pdf_la_SOURCES = test_pdf.c
test_word_la_SOURCES = test_word.c
test_excel_la_SOURCES = test_excel.c
test_powerpoint_la_SOURCES = test_powerpoint.c
test_tar_la_SOURCES = test_tar.c
test_gz_la_SOURCES = test_gz.c
test_zip_la_SOURCES = test_zip.c
test_html_la_SOURCES = test_html.c
test_mail_la_SOURCES = test_mail.c
test_bin_chupatext_la_SOURCES = test_bin_chupatext.c
test_utils_la_SOURCES = test_utils.c

echo-top-builddir:
	@echo $(abs_top_builddir)

echo-top-srcdir:
	@echo $(abs_top_srcdir)

echo-testdir:
	@echo $(abs_srcdir)

echo-cutter:
	@echo $(CUTTER)

echo-ruby:
	@echo $(RUBY)
