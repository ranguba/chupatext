lib_LTLIBRARIES = libchupatext.la

EMPTY =

bin_PROGRAMS = chupa

chupa_SOURCES = chupa.c
chupa_LDADD = libchupatext.la

BUILT_SOURCES = version.h
DISTCLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES = version.h
EXTRA_DIST = version.h

libchupatext_la_SOURCES =			\
	chupa_enum_types.c			\
	chupa_marshalers.c			\
	chupa_marshalers.h			\
	chupa_init.c				\
	chupa_metadata.c			\
	chupa_feeder.c				\
	chupa_text_input.c			\
	chupa_text.c				\
	chupa_module.c				\
	chupa_decomposer_factory.c		\
	chupa_decomposer_description.c		\
	chupa_dispatcher.c			\
	chupa_text_input_stream.c		\
	chupa_memory_input_stream.c		\
	chupa_logger.c				\
	chupa_decomposer.c			\
	archive_decomposer.c			\
	external_decomposer.c			\
	chupa_utils.c				\
	version.c				\
	$(EMPTY)

AM_CPPFLAGS =							\
	-DG_LOG_DOMAIN=\"ChupaText\"				\
	-DCHUPA_LOG_DOMAIN=\"ChupaText\"			\
	-DCHUPA_MODULE_DIR=\"$(moduledir)\"			\
	-DCHUPA_DECOMPOSER_MODULE_DIR=\"$(decomposerdir)\"	\
	-DDESCRIPTIONS_DIR=\"$(descriptionsdir)\"		\
	-D_GNU_SOURCE
AM_CFLAGS = $(GSF_CFLAGS) $(GIO_UNIX_CFLAGS)
AM_LDFLAGS = $(GSF_LIBS) $(GIO_UNIX_LIBS)

DEFAULT_INCLUDES = -I$(top_includedir) -I$(srcdir) -I$(build_includedir) -I.

chupa_public_headers =				\
	archive_decomposer.h			\
	chupa_decomposer.h			\
	chupa_metadata.h			\
	chupa_module.h				\
	chupa_decomposer_factory.h		\
	chupa_decomposer_description.h		\
	chupa_decomposer_module.h		\
	chupa_dispatcher.h			\
	chupa_private.h				\
	chupa_feeder.h				\
	chupa_text_input.h			\
	chupa_text.h				\
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
	$(SHELL) $(top_srcdir)/tool/version-update.sh $@

include $(top_srcdir)/build/enum_types.mk
include $(top_srcdir)/build/marshal.mk
