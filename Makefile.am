ACLOCAL_AMFLAGS = -I m4

SUBDIRS = build data chupatext modules test license po apt

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

BUILT_SOURCES = version.txt
DISTCLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST =					\
	COPYING					\
	README.ja				\
	$(BUILT_SOURCES)

GIT = git

$(top_srcdir)/version.txt:
	(description=$$($(GIT) describe --tags --match='[0-9][.0-9]*');		\
	 n_commits=$$(echo $${description} | cut -d- -f2);			\
	 echo "CHUPA_N_COMMITS=$${n_commits:-0}";				\
	 commit_id=$$(echo $${description} | cut -d- -f3);			\
	 echo "CHUPA_COMMIT_ID=$${commit_id}";					\
	 date=$$($(GIT) log --max-count=1 --format=%cd --date=short | tr -d -);	\
	 echo "CHUPA_RELEASE_DATE=$${date:-0}") > $@.new
	if test ! -f $@ || ! cmp -s $@.new $@; then	\
		touch $(top_srcdir)/configure.ac;	\
	fi
	mv $@.new $@

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
