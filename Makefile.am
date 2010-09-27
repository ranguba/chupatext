ACLOCAL_AMFLAGS = -I m4

SUBDIRS = tool chupatext test module license

pkginclude_HEADERS = chupatext.h

EXTRA_DIST =					\
	COPYING					\
	README.ja

all-recursive: prereq

prereq:

GIT = git

up: pull prereq

pull:
	@cd "$(srcdir)" && $(GIT) pull

prereq: prereq-local prereq-recursive

prereq-local: update-version.h

prereq-recursive:

update-version.h:
	@cd "$(top_srcdir)" && \
	$(SHELL) ./tool/version-update.sh chupatext/version.h

.PHONY: update-version.h prereq-local prereq-recursive
