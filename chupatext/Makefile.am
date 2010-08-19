lib_LTLIBRARIES = libchupatext.la

EMPTY =

libchupatext_la_SOURCES = chupa_init.c chupa_metadata.c \
	chupa_text.c chupa_text_input_stream.c chupa_module.c \
	version.c \
	$(EMPTY)

AM_CFLAGS = $(GLIB_CFLAGS)
DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

version.lo: version.h
