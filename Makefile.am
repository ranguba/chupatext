ACLOCAL_AMFLAGS = -I m4

SUBDIRS = chupatext test module

all-recursive: update-version.h

update-version.h:
	$(SHELL) $(top_srcdir)/tool/version-update.sh chupatext/version.h $(top_srcdir)

.PHONY: update-version.h
