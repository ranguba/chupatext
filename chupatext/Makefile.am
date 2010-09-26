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
	chupa_module_factory_utils.c		\
	chupa_dispatcher.c			\
	chupa_text_input_stream.c		\
	chupa_gsf_input_stream.c		\
	error.c					\
	chupa_decomposer.c			\
	archive_decomposer.c			\
	external_decomposer.c			\
	chupa_restrict_input_stream.c		\
	chupa_utils.c				\
	version.c				\
	$(EMPTY)

AM_CPPFLAGS =						\
	-DCHUPA_MODULE_DIR=\"$(moduledir)\"		\
	-DDECOMPOSER_MODULE_DIR=\"$(decomposerdir)\"	\
	-DFACTORY_MODULE_DIR=\"$(factorydir)\"		\
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
	chupa_module_factory_utils.h		\
	chupa_module_impl.h			\
	chupa_dispatcher.h			\
	chupa_private.h				\
	chupa_restrict_input_stream.h		\
	chupa_text.h				\
	chupa_text_input.h			\
	chupa_text_input_stream.h		\
	chupa_gsf_input_stream.h		\
	external_decomposer.h			\
	chupa_utils.h				\
	version.h

version.lo: version.h
