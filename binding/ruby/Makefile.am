DEFAULT_INCLUDES = -I$(top_includedir) -I$(build_includedir)

LDFLAGS = -avoid-version -module -L$(top_builddir)/chupatext -lchupatext
ARCH_FLAG = $(RUBY_ARCH_FLAG)

module_LTLIBRARIES = chupa.la

CHUPA_RUBY_CFLAGS = $(RUBY_CFLAGS)
CHUPA_RUBY_CPPFLAGS =				\
	$(RUBY_CPPFLAGS)			\
	$(GLIB_CFLAGS)				\
	$(GSF_CFLAGS)
CHUPA_RUBY_LIBADD = $(RUBY_LDFLAGS) $(RUBY_LIBS) $(GSF_LIBS)

chupa_la_SOURCES = init.c metadata.c chupa_ruby.h
chupa_la_CFLAGS = $(CHUPA_RUBY_CFLAGS)
chupa_la_CPPFLAGS =			\
	$(CHUPA_RUBY_CPPFLAGS)			\
	$(EMPTY)
chupa_la_LIBADD = $(CHUPA_RUBY_LIBADD)
