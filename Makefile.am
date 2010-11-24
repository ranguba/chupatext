ACLOCAL_AMFLAGS = -I m4 $ACLOCAL_OPTIONS

SUBDIRS = build data chupatext modules test license po misc doc apt

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA =				\
	chupatext.pc				\
	chupatext-excel.pc			\
	chupatext-pdf.pc			\
	chupatext-ruby.pc			\
	chupatext-tar.pc			\
	chupatext-text.pc			\
	chupatext-word.pc			\
	chupatext-zip.pc			\
	chupatext-gzip.pc

pkginclude_HEADERS = chupatext.h

BUILT_SOURCES = version.txt
DISTCLEANFILES = $(BUILT_SOURCES)
MAINTAINERCLEANFILES = $(BUILT_SOURCES)
EXTRA_DIST =					\
	COPYING					\
	README.ja				\
	NEWS					\
	NEWS.ja					\
	$(BUILT_SOURCES)

GIT = git

dist: regenerate-version.txt

regenerate-version.txt:
	rm -f $(top_srcdir)/version.txt
	$(MAKE) $(top_srcdir)/version.txt

$(top_srcdir)/version.txt:
	(description=$$($(GIT) describe --tags --match='[0-9][.0-9]*');		\
	 if echo $${description} | grep -- - > /dev/null 2>&1 ; then		\
	   n_commits=$$(echo $${description} | cut -d- -f2);			\
	   commit_id=$$(echo $${description} | cut -d- -f3);			\
	 fi;									\
	 echo "CHUPA_N_COMMITS=$${n_commits:-0}";				\
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
	   find modules -name '*.c') |		\
	  sort > po/POTFILES.in
	cd po && $(MAKE) $(AM_MAKEFLAGS) update-po

echo-top-builddir:
	@echo $(top_builddir)

upload: upload-doc

upload-doc:
	cd doc/reference && $(MAKE) upload

tag:
	git tag -a ${VERSION} -m "release ${VERSION}!!!"

update-latest-release:
	@if test -z "$(OLD_RELEASE)"; then	\
	  echo "\$$(OLD_RELEASE) is missing";	\
	  exit 1;				\
	fi
	@if test -z "$(OLD_RELEASE_DATE)"; then		\
	  echo "\$$(OLD_RELEASE_DATE) is missing";	\
	  exit 1;					\
	fi
	@if test -z "$(NEW_RELEASE_DATE)"; then		\
	  echo "\$$(NEW_RELEASE_DATE) is missing";	\
	  exit 1;					\
	fi
	cd $(top_srcdir) &&					\
	  misc/update-latest-release.rb				\
	  $(PACKAGE) $(OLD_RELEASE) $(OLD_RELEASE_DATE)		\
	  $(VERSION) $(NEW_RELEASE_DATE)			\
	  debian/changelog

release: dist
	rubyforge add_release $(RUBYFORGE_PROJECT_NAME) $(PACKAGE) \
	  $(VERSION) $(PACKAGE)-$(VERSION).tar.gz
