ACLOCAL_AMFLAGS = -I m4

SUBDIRS = tool chupatext test module license

pkginclude_HEADERS = chupatext.h

EXTRA_DIST =					\
	COPYING					\
	README.ja

GIT = git

up: pull

pull:
	@cd "$(srcdir)" && $(GIT) pull
