EMPTY =

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

LDFLAGS = -module -L$(top_builddir)/chupatext -lchupatext

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

text_la_SOURCES = text.c
text_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
text_la_LIBADD = $(GLIB_LIBS)

text_factory_la_SOURCES = text_factory.c
text_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
text_factory_la_LIBADD = $(GLIB_LIBS)

tar_la_SOURCES = tar.c
tar_la_CPPFLAGS = $(GSF_CFLAGS)
tar_la_LIBADD = $(GSF_LIBS)

tar_factory_la_SOURCES = tar_factory.c
tar_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
tar_factory_la_LIBADD = $(GLIB_LIBS)

zip_la_SOURCES = zip.c
zip_la_CPPFLAGS = $(GSF_CFLAGS)
zip_la_LIBADD = $(GSF_LIBS)

zip_factory_la_SOURCES = zip_factory.c
zip_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
zip_factory_la_LIBADD = $(GLIB_LIBS)

pdf_la_SOURCES = pdf.c
pdf_la_CPPFLAGS = $(POPPLER_GLIB_CFLAGS) $(GSF_CFLAGS)
pdf_la_LIBADD = $(POPPLER_GLIB_LIBS) $(GSF_LIBS)

pdf_factory_la_SOURCES = pdf_factory.c
pdf_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
pdf_factory_la_LIBADD = $(GLIB_LIBS)

word_la_SOURCES = word.c
word_la_CPPFLAGS = $(WV_CFLAGS)
word_la_LIBADD = $(WV_LIBS) $(GSF_LIBS)

word_factory_la_SOURCES = word_factory.c
word_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
word_factory_la_LIBADD = $(GLIB_LIBS)

excel_la_SOURCES = excel.c
excel_la_CPPFLAGS = -I$(srcdir)/excel $(GOFFICE_CFLAGS)
excel_la_LIBADD = $(GOFFICE_LIBS) -lspreadsheet

excel_factory_la_SOURCES = excel_factory.c
excel_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
excel_factory_la_LIBADD = $(GLIB_LIBS)

powerpoint_la_SOURCES = powerpoint.c
powerpoint_la_CPPFLAGS = $(GIO_UNIX_CFLAGS) $(GSF_CFLAGS)
powerpoint_la_LIBADD = $(GIO_UNIX_LIBS) $(GSF_LIBS)

powerpoint_factory_la_SOURCES = powerpoint_factory.c
powerpoint_factory_la_CPPFLAGS = $(GLIB_CFLAGS) $(GSF_CFLAGS)
powerpoint_factory_la_LIBADD = $(GLIB_LIBS)
