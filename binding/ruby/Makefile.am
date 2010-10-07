DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

LDFLAGS = -L$(top_builddir)/chupatext -lchupatext

lib_LTLIBRARIES = libchuparuby.la

CHUPA_RUBY_CFLAGS = $(RUBY_CFLAGS)
CHUPA_RUBY_CPPFLAGS =				\
	$(RUBY_CPPFLAGS)			\
	$(GLIB_CFLAGS)				\
	$(GSF_CFLAGS)
CHUPA_RUBY_LIBADD = $(RUBY_LDFLAGS) $(RUBY_LIBS) $(GSF_LIBS)

libchuparuby_la_SOURCES = chupa_ruby.c
libchuparuby_la_CFLAGS = $(CHUPA_RUBY_CFLAGS)
libchuparuby_la_CPPFLAGS =			\
	$(CHUPA_RUBY_CPPFLAGS)			\
	-DCHUPA_RUBY_DIR=\"$(rubydir)\"		\
	-DRUBY_ARCH=\"$(rubyarch)\"		\
	$(EMPTY)
libchuparuby_la_LIBADD = $(CHUPA_RUBY_LIBADD)
