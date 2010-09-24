#!/usr/bin/make -f

prereq:

GIT = git

up: pull prereq

pull:
	@cd "$(srcdir)" && $(GIT) pull

prereq: prereq-local prereq-recursive

prereq-local: update-version.h

prereq-recursive:
	@local_target=`echo $@ | sed s/-recursive//`; \
	top="$(top_srcdir)"; \
	: "$${top:=.}"; \
	set dummy $(SUBDIRS); shift; \
	if test $$# = 0; then \
	    set dummy `ls */prereq.mk 2> /dev/null | sed 's/\/prereq\.mk$$//'`; \
	    shift; \
	fi; \
	for subdir do \
	    test -f "$$top/$$subdir/prereq.mk" || continue; \
	    (if test -z "$(am__cd)"; then cd $$subdir; else $(am__cd) $$subdir; fi && \
	    if test -f Makefile; then \
		exec $(MAKE) $(AM_MAKEFLAGS) $$local_target; \
	    else \
		exec $(MAKE) -f prereq.mk $(AM_MAKEFLAGS) $$local_target; \
	    fi) || exit 1; \
	done

update-version.h:
	@cd "$(top_srcdir)" && \
	$(SHELL) ./tool/version-update.sh chupatext/version.h

.PHONY: update-version.h prereq-local prereq-recursive
