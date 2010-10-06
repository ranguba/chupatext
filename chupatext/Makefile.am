lib_LTLIBRARIES = libchupatext.la

EMPTY =

bin_PROGRAMS = chupa

chupa_SOURCES = chupa.c
chupa_LDADD = libchupatext.la

BUILT_SOURCES = version.h
MAINTAINERCLEANFILES = version.h
EXTRA_DIST = version.h

libchupatext_la_SOURCES =			\
	chupa_enum_types.c			\
	chupa_marshalers.c			\
	chupa_marshalers.h			\
	chupa_init.c				\
	chupa_metadata.c			\
	chupa_text.c				\
	chupa_text_input.c			\
	chupa_module.c				\
	chupa_module_factory.c			\
	chupa_module_factory_utils.c		\
	chupa_module_description.c		\
	chupa_dispatcher.c			\
	chupa_text_input_stream.c		\
	chupa_memory_input_stream.c		\
	chupa_logger.c				\
	error.c					\
	chupa_decomposer.c			\
	archive_decomposer.c			\
	external_decomposer.c			\
	chupa_utils.c				\
	version.c				\
	$(EMPTY)

AM_CPPFLAGS =						\
	-DCHUPA_MODULE_DIR=\"$(moduledir)\"		\
	-DDECOMPOSER_MODULE_DIR=\"$(decomposerdir)\"	\
	-DFACTORY_MODULE_DIR=\"$(factorydir)\"		\
	-DDESCRIPTIONS_DIR=\"$(descriptionsdir)\"	\
	-D_GNU_SOURCE
AM_CFLAGS = $(GSF_CFLAGS) $(GIO_UNIX_CFLAGS)
AM_LDFLAGS = $(GSF_LIBS) $(GIO_UNIX_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

chupa_public_headers =				\
	archive_decomposer.h			\
	chupa_decomposer.h			\
	chupa_metadata.h			\
	chupa_module.h				\
	chupa_module_factory.h			\
	chupa_module_factory_utils.h		\
	chupa_module_description.h		\
	chupa_module_impl.h			\
	chupa_dispatcher.h			\
	chupa_private.h				\
	chupa_text.h				\
	chupa_text_input.h			\
	chupa_text_input_stream.h		\
	chupa_memory_input_stream.h		\
	chupa_logger.h				\
	external_decomposer.h			\
	chupa_utils.h				\
	version.h

chupatext_includedir = $(pkgincludedir)/chupatext
chupatext_include_HEADERS =			\
	chupa_enum_types.h			\
	$(chupa_public_headers)

enum_source_prefix = chupa_enum_types
enum_sources_h = $(chupa_public_headers)

marshal_source_prefix = chupa_marshalers
marshal_prefix = _chupa_marshal

version.h: $(top_srcdir)/tool/version-update.sh
	$(SHELL) $(top_srcdir)/tool/version-update.sh version.h

include $(top_srcdir)/build/enum_types.mk
include $(top_srcdir)/build/marshal.mk
