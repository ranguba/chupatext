SUBDIRS = excel ruby

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

decomposer_LTLIBRARIES += text.la
decomposer_LTLIBRARIES += tar.la
decomposer_LTLIBRARIES += zip.la
if HAVE_POPPLER_GLIB
decomposer_LTLIBRARIES += pdf.la
endif
if HAVE_WV
decomposer_LTLIBRARIES += word.la
endif
if HAVE_GOFFICE
decomposer_LTLIBRARIES += excel.la
endif

if HAVE_RUBY
decomposer_LTLIBRARIES += ruby.la
endif

text_la_SOURCES = text.c
text_la_CPPFLAGS =				\
	$(GLIB_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Text\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Text\"
text_la_LIBADD = $(chupa_libs)

tar_la_SOURCES = tar.c
tar_la_CPPFLAGS =				\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Tar\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Tar\"
tar_la_LIBADD = $(chupa_libs)

zip_la_SOURCES = zip.c
zip_la_CPPFLAGS =				\
	$(GSF_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Zip\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Zip\"
zip_la_LIBADD = $(chupa_libs)

pdf_la_SOURCES = pdf.c
pdf_la_CPPFLAGS =				\
	$(POPPLER_GLIB_CFLAGS)			\
	-DG_LOG_DOMAIN=\"ChupaText-PDF\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-PDF\"
pdf_la_LIBADD = $(chupa_libs) $(POPPLER_GLIB_LIBS)

word_la_SOURCES = word.c
word_la_CPPFLAGS =				\
	$(WV_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Word\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Word\"
word_la_LIBADD = $(chupa_libs) $(WV_LIBS)

excel_la_SOURCES = excel.c
excel_la_CPPFLAGS =				\
	$(GOFFICE_CFLAGS)			\
	$(GTK_CFLAGS)				\
	-DG_LOG_DOMAIN=\"ChupaText-Excel\"	\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Excel\"
excel_la_LIBADD = $(chupa_libs) $(GOFFICE_LIBS) $(GTK_LIBS) -lspreadsheet

ruby_la_SOURCES = ruby.c
ruby_la_CFLAGS = $(RUBY_CFLAGS)
ruby_la_CPPFLAGS =					\
	-I$(srcdir)/ruby				\
	-DCHUPA_RUBY_LIBRARY_DIR=\"$(ruby_libdir)\"	\
	-DG_LOG_DOMAIN=\"ChupaText-Ruby\"		\
	-DCHUPA_LOG_DOMAIN=\"ChupaText-Ruby\"		\
	$(RUBY_CPPFLAGS)				\
	$(RUBY_GLIB2_CFLAGS)				\
	$(GLIB_CFLAGS)
ruby_la_LIBADD =				\
	$(chupa_libs)				\
	$(RUBY_LDFLAGS)				\
	$(RUBY_LIBS)				\
	$(builddir)/ruby/libchuparuby.la
