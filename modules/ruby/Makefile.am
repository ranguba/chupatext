SUBDIRS = lib

AM_LDFLAGS =					\
	-L$(top_builddir)/chupatext
AM_CPPFLAGS =					\
	-I$(top_includedir)			\
	-I$(build_includedir)			\
	$(RUBY_CPPFLAGS)			\
	$(GLIB_CFLAGS)				\
	$(GSF_CFLAGS)
AM_CFLAGS =					\
	$(RUBY_CFLAGS)				\
	$(RUBY_GLIB2_CFLAGS)
ARCH_FLAG = $(RUBY_ARCH_FLAG)

if HAVE_RUBY
noinst_LTLIBRARIES = libchuparuby.la
endif

libchuparuby_la_SOURCES =			\
	init.c					\
	metadata.c				\
	feeder.c				\
	data.c					\
	decomposer.c				\
	error.c					\
	g_memory_input_stream.c			\
	utils.c					\
	types.c					\
	logger.c				\
	chupa_ruby.h
libchuparuby_la_LIBADD =				\
	$(RUBY_LDFLAGS)					\
	$(RUBY_LIBS)					\
	$(GSF_LIBS)					\
	$(top_builddir)/chupatext/libchupatext.la
