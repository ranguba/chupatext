SUBDIRS = excel

EMPTY =

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

binding_dir = $(top_builddir)/binding
AM_LDFLAGS =					\
	-avoid-version				\
	-no-undefined				\
	-module

chupa_libs =						\
	$(top_builddir)/chupatext/libchupatext.la

decomposer_LTLIBRARIES =
factory_LTLIBRARIES =

decomposer_LTLIBRARIES += text.la
factory_LTLIBRARIES += text_factory.la
decomposer_LTLIBRARIES += tar.la
factory_LTLIBRARIES += tar_factory.la
decomposer_LTLIBRARIES += zip.la
factory_LTLIBRARIES += zip_factory.la
if HAVE_POPPLER_GLIB
decomposer_LTLIBRARIES += pdf.la
factory_LTLIBRARIES += pdf_factory.la
endif
if HAVE_WV
decomposer_LTLIBRARIES += word.la
factory_LTLIBRARIES += word_factory.la
endif
if HAVE_GOFFICE
decomposer_LTLIBRARIES += excel.la
factory_LTLIBRARIES += excel_factory.la
endif
decomposer_LTLIBRARIES += powerpoint.la
factory_LTLIBRARIES += powerpoint_factory.la
if HAVE_RUBY
decomposer_LTLIBRARIES += ruby.la
factory_LTLIBRARIES += ruby_factory.la
endif
if FALSE
decomposer_LTLIBRARIES += html.la
factory_LTLIBRARIES += html_factory.la
endif

text_la_SOURCES = text.c
text_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)				\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Text\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Text\"
text_la_LIBADD = $(chupa_libs)

text_factory_la_SOURCES = text_factory.c
text_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-TextFactory\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-TextFactory\"
text_factory_la_LIBADD = $(chupa_libs)

tar_la_SOURCES = tar.c
tar_la_CPPFLAGS =				\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Tar\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Tar\"
tar_la_LIBADD = $(chupa_libs)

tar_factory_la_SOURCES = tar_factory.c
tar_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-TarFactory\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-TarFactory\"
tar_factory_la_LIBADD = $(chupa_libs)

zip_la_SOURCES = zip.c
zip_la_CPPFLAGS =				\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Zip\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Zip\"
zip_la_LIBADD = $(chupa_libs)

zip_factory_la_SOURCES = zip_factory.c
zip_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-ZipFactory\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-ZipFactory\"
zip_factory_la_LIBADD = $(chupa_libs)

pdf_la_SOURCES = pdf.c
pdf_la_CPPFLAGS =				\
	$(POPPLER_GLIB_CFLAGS)			\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-PDF\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-PDF\"
pdf_la_LIBADD = $(chupa_libs) $(POPPLER_GLIB_LIBS)

pdf_factory_la_SOURCES = pdf_factory.c
pdf_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-PDFFactory\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-PDFFactory\"
pdf_factory_la_LIBADD = $(chupa_libs)

word_la_SOURCES = word.c
word_la_CPPFLAGS =				\
	$(WV_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Word\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Word\"
word_la_LIBADD = $(chupa_libs) $(WV_LIBS)

word_factory_la_SOURCES = word_factory.c
word_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-WordFactory\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-WordFactory\"
word_factory_la_LIBADD = $(chupa_libs)

excel_la_SOURCES = excel.c
excel_la_CPPFLAGS =				\
	$(GOFFICE_CFLAGS)			\
	-DG_LOG_DOMAIN=\"ChupaText-Excel\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Excel\"
excel_la_LIBADD = $(chupa_libs) $(GOFFICE_LIBS) -lspreadsheet

excel_factory_la_SOURCES = excel_factory.c
excel_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-ExcelFactory\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-ExcelFactory\"
excel_factory_la_LIBADD = $(chupa_libs)

powerpoint_la_SOURCES = powerpoint.c
powerpoint_la_CPPFLAGS =				\
	$(GIO_UNIX_CFLAGS)				\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-PowerPoint\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-PowerPoint\"
powerpoint_la_LIBADD = $(chupa_libs) $(GIO_UNIX_LIBS)

powerpoint_factory_la_SOURCES = powerpoint_factory.c
powerpoint_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)						\
	$(GSF_CFLAGS)						\
	-DG_LOG_DOMAIN=\"ChupaText-PowerPointFactory\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-PowerPointFactory\"
powerpoint_factory_la_LIBADD = $(chupa_libs)

CHUPA_RUBY_CFLAGS = $(RUBY_CFLAGS)
CHUPA_RUBY_CPPFLAGS =				\
	-I$(top_srcdir)/binding/ruby		\
	-DCHUPA_RUBY_DIR=\"$(rubydir)\"		\
	-DRUBY_ARCH=\"$(rubyarch)\"		\
	$(RUBY_CPPFLAGS)			\
	$(GLIB_CFLAGS)				\
	$(GSF_CFLAGS)
CHUPA_RUBY_LIBADD = $(chupa_libs) $(RUBY_LDFLAGS) $(RUBY_LIBS)

ruby_la_SOURCES = ruby.c
ruby_la_CFLAGS = $(CHUPA_RUBY_CFLAGS)
ruby_la_CPPFLAGS =				\
	$(CHUPA_RUBY_CPPFLAGS)			\
	-DG_LOG_DOMAIN=\"ChupaText-Ruby\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Ruby\"
ruby_la_LIBADD = $(CHUPA_RUBY_LIBADD)

ruby_factory_la_SOURCES = ruby_factory.c
ruby_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-RubyFactory\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-RubyFactory\"
ruby_factory_la_LIBADD = $(chupa_libs)

if FALSE
html_la_SOURCES = html.c
html_la_CFLAGS = $(CHUPA_RUBY_CFLAGS)
html_la_CPPFLAGS =				\
	$(CHUPA_RUBY_CPPFLAGS)			\
	-DG_LOG_DOMAIN=\"ChupaText-Html\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Html\"
html_la_LIBADD = $(CHUPA_RUBY_LIBADD)

html_factory_la_SOURCES = html_factory.c
html_factory_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)					\
	$(GSF_CFLAGS)					\
	-DG_LOG_DOMAIN=\"ChupaText-HtmlFactory\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-HtmlFactory\"
html_factory_la_LIBADD = $(chupa_libs)
endif
