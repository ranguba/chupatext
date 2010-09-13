lib_LTLIBRARIES = libchupatext.la

EMPTY =

libchupatext_la_SOURCES = chupa_init.c chupa_metadata.c \
	chupa_text.c chupa_text_input.c chupa_module.c \
	chupa_text_input_stream.c error.c \
	chupa_decomposer.c archive_decomposer.c text_decomposer.c \
	chupa_restrict_input_stream.c \
	version.c \
	$(EMPTY)

AM_CPPFLAGS = -DCHUPA_MODULE_PATH=\"$(moduledir)\" -D_GNU_SOURCE
AM_CFLAGS = $(GSF_CFLAGS)
AM_LDFLAGS = $(GSF_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

version.lo: version.h
