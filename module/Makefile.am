EMPTY =

LDFLAGS = -L$(top_builddir)/chupatext -lchupatext

decomposer_LTLIBRARIES =
factory_LTLIBRARIES =
decomposer_LTLIBRARIES += chupa_tar.la
decomposer_LTLIBRARIES += chupa_zip.la
if HAVE_POPPLER_GLIB
decomposer_LTLIBRARIES += pdf.la
factory_LTLIBRARIES += pdf_factory.la
endif
if HAVE_WV
decomposer_LTLIBRARIES += chupa_msword.la
endif
if HAVE_GOFFICE
decomposer_LTLIBRARIES += chupa_msexcel.la
endif
decomposer_LTLIBRARIES += chupa_mspowerpoint.la

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

chupa_msword_la_SOURCES = chupa_msword.c
chupa_msword_la_LDFLAGS = -module
chupa_msword_la_CPPFLAGS = $(WV_CFLAGS)
chupa_msword_la_LIBADD = $(WV_LIBS) $(GSF_LIBS)

chupa_msexcel_la_SOURCES = chupa_msexcel.c
chupa_msexcel_la_LDFLAGS = -module
chupa_msexcel_la_CPPFLAGS = -I$(srcdir)/msexcel $(GOFFICE_CFLAGS)
chupa_msexcel_la_LIBADD = $(GOFFICE_LIBS) -lspreadsheet

chupa_mspowerpoint_la_SOURCES = chupa_mspowerpoint.c
chupa_mspowerpoint_la_LDFLAGS = -module
chupa_mspowerpoint_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_mspowerpoint_la_LIBADD = $(GSF_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

all: prereq

@am__include@ $(srcdir)/prereq.mk
