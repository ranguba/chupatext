EMPTY =

LDFLAGS = -L$(top_builddir)/chupatext -lchupatext

decomposer_LTLIBRARIES =
factory_LTLIBRARIES =

decomposer_LTLIBRARIES += text.la
factory_LTLIBRARIES += text_factory.la
decomposer_LTLIBRARIES += chupa_tar.la
decomposer_LTLIBRARIES += chupa_zip.la
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
decomposer_LTLIBRARIES += chupa_mspowerpoint.la

text_la_SOURCES = text.c
text_la_LDFLAGS = -module
text_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
text_la_LIBADD = $(GLIB_LIBS)

text_factory_la_SOURCES = text_factory.c
text_factory_la_LDFLAGS = -module
text_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
text_factory_la_LIBADD = $(GLIB_LIBS)

chupa_tar_la_SOURCES = chupa_tar.c
chupa_tar_la_LDFLAGS = -module
chupa_tar_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_tar_la_LIBADD = $(GSF_LIBS)

chupa_zip_la_SOURCES = chupa_zip.c
chupa_zip_la_LDFLAGS = -module
chupa_zip_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_zip_la_LIBADD = $(GSF_LIBS)

pdf_la_SOURCES = pdf.c
pdf_la_LDFLAGS = -module
pdf_la_CPPFLAGS = $(POPPLER_GLIB_CFLAGS) $(GSF_CFLAGS)
pdf_la_LIBADD = $(POPPLER_GLIB_LIBS) $(GSF_LIBS)

pdf_factory_la_SOURCES = pdf_factory.c
pdf_factory_la_LDFLAGS = -module
pdf_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
pdf_factory_la_LIBADD = $(GLIB_LIBS)

word_la_SOURCES = word.c
word_la_LDFLAGS = -module
word_la_CPPFLAGS = $(WV_CFLAGS)
word_la_LIBADD = $(WV_LIBS) $(GSF_LIBS)

word_factory_la_SOURCES = word_factory.c
word_factory_la_LDFLAGS = -module
word_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
word_factory_la_LIBADD = $(GLIB_LIBS)

excel_la_SOURCES = excel.c
excel_la_LDFLAGS = -module
excel_la_CPPFLAGS = -I$(srcdir)/excel $(GOFFICE_CFLAGS)
excel_la_LIBADD = $(GOFFICE_LIBS) -lspreadsheet

excel_factory_la_SOURCES = excel_factory.c
excel_factory_la_LDFLAGS = -module
excel_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
excel_factory_la_LIBADD = $(GLIB_LIBS)

chupa_mspowerpoint_la_SOURCES = chupa_mspowerpoint.c
chupa_mspowerpoint_la_LDFLAGS = -module
chupa_mspowerpoint_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_mspowerpoint_la_LIBADD = $(GSF_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

all: prereq

@am__include@ $(srcdir)/prereq.mk
