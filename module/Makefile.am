EMPTY =

LDFLAGS = -L$(top_builddir)/chupatext -lchupatext

module_LTLIBRARIES =
module_LTLIBRARIES += chupa_tar.la
if HAVE_POPPLER_GLIB
module_LTLIBRARIES += chupa_pdf.la
endif
if HAVE_WV
module_LTLIBRARIES += chupa_msword.la
endif

chupa_tar_la_SOURCES = chupa_tar.c
chupa_tar_la_LDFLAGS = -module
chupa_tar_la_CPPFLAGS = $(GLIB_CFLAGS)
chupa_tar_la_LIBADD = $(GLIB_LIBS)

chupa_pdf_la_SOURCES = chupa_pdf.c
chupa_pdf_la_LDFLAGS = -module
chupa_pdf_la_CPPFLAGS = $(POPPLER_GLIB_CFLAGS)
chupa_pdf_la_LIBADD = $(POPPLER_GLIB_LIBS)

chupa_msword_la_SOURCES = chupa_msword.c
chupa_msword_la_LDFLAGS = -module
chupa_msword_la_CPPFLAGS = $(WV_CFLAGS)
chupa_msword_la_LIBADD = $(WV_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)
