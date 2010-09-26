lib_LTLIBRARIES = libchupatext.la

EMPTY =

bin_PROGRAMS = chupa

chupa_SOURCES = chupa.c
chupa_LDADD = libchupatext.la

libchupatext_la_SOURCES =			\
	chupa_init.c				\
	chupa_metadata.c			\
	chupa_text.c				\
	chupa_text_input.c			\
	chupa_module.c				\
	chupa_module_factory.c			\
	chupa_text_input_stream.c		\
	chupa_gsf_input_stream.c		\
	error.c					\
	chupa_decomposer.c			\
	archive_decomposer.c			\
	external_decomposer.c			\
	text_decomposer.c			\
	chupa_restrict_input_stream.c		\
	version.c				\
	$(EMPTY)

AM_CPPFLAGS =					\
	-DCHUPA_MODULE_DIR=\"$(moduledir)\"	\
	-D_GNU_SOURCE
AM_CFLAGS = $(GSF_CFLAGS) $(GIO_UNIX_CFLAGS)
AM_LDFLAGS = $(GSF_LIBS) $(GIO_UNIX_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

chupatext_includedir = $(pkgincludedir)/chupatext
chupatext_include_HEADERS =			\
	archive_decomposer.h			\
	chupa_decomposer.h			\
	chupa_metadata.h			\
	chupa_module.h				\
	chupa_module_factory.h			\
	chupa_module_impl.h			\
	chupa_private.h				\
	chupa_restrict_input_stream.h		\
	chupa_text.h				\
	chupa_text_input.h			\
	chupa_text_input_stream.h		\
	chupa_gsf_input_stream.h		\
	text_decomposer.h			\
	external_decomposer.h			\
	version.h

version.lo: version.h
