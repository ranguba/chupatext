EMPTY =

AM_CFLAGS = $(GSF_CFLAGS)
AM_LDFLAGS = $(GSF_LIBS)

LDFLAGS = -L$(top_builddir)/chupatext -lchupatext

module_LTLIBRARIES =
module_LTLIBRARIES += chupa_tar.la
module_LTLIBRARIES += chupa_zip.la
if HAVE_POPPLER_GLIB
module_LTLIBRARIES += chupa_pdf.la
endif
if HAVE_WV
module_LTLIBRARIES += chupa_msword.la
endif
module_LTLIBRARIES += chupa_mspowerpoint.la

chupa_tar_la_SOURCES = chupa_tar.c
chupa_tar_la_LDFLAGS = -module
chupa_tar_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_tar_la_LIBADD = $(GSF_LIBS)

chupa_zip_la_SOURCES = chupa_zip.c
chupa_zip_la_LDFLAGS = -module
chupa_zip_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_zip_la_LIBADD = $(GSF_LIBS)

chupa_pdf_la_SOURCES = chupa_pdf.c
chupa_pdf_la_LDFLAGS = -module
chupa_pdf_la_CPPFLAGS = $(POPPLER_GLIB_CFLAGS)
chupa_pdf_la_LIBADD = $(POPPLER_GLIB_LIBS) $(GSF_LIBS)

chupa_msword_la_SOURCES = chupa_msword.c
chupa_msword_la_LDFLAGS = -module
chupa_msword_la_CPPFLAGS = $(WV_CFLAGS)
chupa_msword_la_LIBADD = $(WV_LIBS) $(GSF_LIBS)

chupa_mspowerpoint_la_SOURCES = chupa_mspowerpoint.c
chupa_mspowerpoint_la_LDFLAGS = -module
chupa_mspowerpoint_la_CPPFLAGS = $(GSF_CFLAGS)
chupa_mspowerpoint_la_LIBADD = $(GSF_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

all: prereq

@am__include@ $(srcdir)/prereq.mk
