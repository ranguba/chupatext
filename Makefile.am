ACLOCAL_AMFLAGS = -I m4

SUBDIRS = build tool data chupatext test module license po apt

pkginclude_HEADERS = chupatext.h

EXTRA_DIST =					\
	COPYING					\
	README.ja

GIT = git

up: pull

pull:
	@cd "$(srcdir)" && $(GIT) pull

update-po:
	cd $(top_srcdir) &&			\
	  (find chupatext -name '*.c';		\
	   find module -name '*.c') |		\
	  sort > po/POTFILES.in
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

echo-top-builddir:
	@echo $(top_builddir)
