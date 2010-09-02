#!/usr/bin/make -f

GIT = git

up: pull update-version.h

pull:
	@cd "$(srcdir)" && $(GIT) pull

update-version.h:
	@cd "$(top_srcdir)" && \
	$(SHELL) ./tool/version-update.sh chupatext/version.h

.PHONY: update-version.h
