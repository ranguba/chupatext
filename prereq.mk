#!/usr/bin/make -f

GIT = git

up: pull update-version.h chupatext/module_list.h

pull:
	@cd "$(srcdir)" && $(GIT) pull

prereq: prereq-local prereq-recursive

prereq-local: update-version.h

prereq-recursive:
	@local_target=`echo $@ | sed s/-recursive//`; \
	top=$(top_srcdir) cd=$(am__cd); \
	: "$${top:=.}" "$${cd:=cd}"; \
	set dummy $(SUBDIRS); shift; \
	if test $$# = 0; then \
	    set dummy `ls */prereq.mk 2> /dev/null | sed 's/\/prereq\.mk$$//'`; \
	    shift; \
	fi; \
	for subdir do \
	    test -f "$$top/$$subdir/prereq.mk" || continue; \
	    ($$cd $$subdir && \
	    if test -f Makefile; then \
		exec $(MAKE) $(AM_MAKEFLAGS) $$local_target; \
	    else \
		exec $(MAKE) -f prereq.mk $(AM_MAKEFLAGS) $$local_target; \
	    fi) || exit 1; \
	done

update-version.h:
	@cd "$(top_srcdir)" && \
	$(SHELL) ./tool/version-update.sh chupatext/version.h

.PHONY: update-version.h
