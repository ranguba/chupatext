EMPTY =

BUILT_SOURCES =
DISTCLEANFILES =
MAINTAINERCLEANFILES =
EXTRA_DIST =

DEFAULT_INCLUDES = -I$(top_includedir) -I$(srcdir) -I$(build_includedir) -I.

AM_CPPFLAGS =							\
	-DG_LOG_DOMAIN=\"ChupaText\"				\
	-DCHUPA_LOG_DOMAIN=\"ChupaText\"			\
	-DCHUPA_MODULES_DIR=\"$(modulesdir)\"			\
	-DCHUPA_DECOMPOSERS_MODULE_DIR=\"$(decomposersdir)\"	\
	-DDESCRIPTIONS_DIR=\"$(descriptionsdir)\"		\
	-D_GNU_SOURCE
AM_CFLAGS =					\
	$(GSF_CFLAGS)				\
	$(GIO_UNIX_CFLAGS)

lib_LTLIBRARIES = libchupatext.la

bin_PROGRAMS = chupatext

chupatext_SOURCES = chupatext.c
chupatext_LDADD = libchupatext.la

libchupatext_la_SOURCES =			\
	chupa_enum_types.c			\
	chupa_marshalers.c			\
	chupa_marshalers.h			\
	chupa_init.c				\
	chupa_metadata.c			\
	chupa_feeder.c				\
	chupa_data.c				\
	chupa_data_input.c			\
	chupa_module.c				\
	chupa_decomposer_factory.c		\
	chupa_decomposer_description.c		\
	chupa_dispatcher.c			\
	chupa_gsf_input_stream.c		\
	chupa_memory_input_stream.c		\
	chupa_logger.c				\
	chupa_decomposer.c			\
	chupa_archive_decomposer.c		\
	chupa_utils.c				\
	chupa_version.c				\
	chupa_types.c				\
	$(EMPTY)

libchupatext_la_LDFLAGS =			\
	-version-info $(LT_VERSION_INFO)	\
	-no-undefined				\
	$(GSF_LIBS)				\
	$(GIO_UNIX_LIBS)

chupa_public_headers =				\
	chupa_archive_decomposer.h		\
	chupa_decomposer.h			\
	chupa_init.h				\
	chupa_metadata.h			\
	chupa_module.h				\
	chupa_decomposer_factory.h		\
	chupa_decomposer_description.h		\
	chupa_decomposer_module.h		\
	chupa_dispatcher.h			\
	chupa_private.h				\
	chupa_feeder.h				\
	chupa_data.h				\
	chupa_data_input.h			\
	chupa_gsf_input_stream.h		\
	chupa_memory_input_stream.h		\
	chupa_logger.h				\
	chupa_utils.h				\
	chupa_version.h

chupatext_includedir = $(pkgincludedir)/chupatext
chupatext_include_HEADERS =			\
	chupa_enum_types.h			\
	$(chupa_public_headers)

enum_source_prefix = chupa_enum_types
enum_sources_h = $(chupa_public_headers)

marshal_source_prefix = chupa_marshalers
marshal_prefix = _chupa_marshal

include $(top_srcdir)/build/enum_types.mk
include $(top_srcdir)/build/marshal.mk
