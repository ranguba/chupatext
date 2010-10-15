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

noinst_LTLIBRARIES = libchuparuby.la

libchuparuby_la_SOURCES =			\
	init.c					\
	metadata.c				\
	feeder.c				\
	chupa_ruby.h
libchuparuby_la_LIBADD =				\
	$(RUBY_LDFLAGS)					\
	$(RUBY_LIBS)					\
	$(GSF_LIBS)					\
	$(top_builddir)/chupatext/libchupatext.la
