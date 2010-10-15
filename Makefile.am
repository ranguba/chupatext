ACLOCAL_AMFLAGS = -I m4

SUBDIRS = build tool data chupatext test binding module license po apt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =				\
	chupatext.pc				\
	chupatext-excel.pc			\
	chupatext-pdf.pc			\
	chupatext-powerpoint.pc			\
	chupatext-ruby.pc			\
	chupatext-tar.pc			\
	chupatext-text.pc			\
	chupatext-word.pc			\
	chupatext-zip.pc

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
