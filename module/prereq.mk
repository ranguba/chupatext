prereq: init_chupa.h

init_chupa.h: $(SOURCES)
	@set dummy $(SOURCES); shift; \
	test $$# -gt 0 || set chupa_*.c; \
	srcdir="$(srcdir)"; : $${srcdir:=.}; \
	for f do \
	    b=`basename $$f .c`; \
	    sed -n '/chupa_decomposer_register/s/.*\(".*"\).*/REGISTER(\1,"'$$b'");/p' \
		"$$srcdir/$$f"; \
	done > $@.new && \
	if test -f $@ && cmp $@ $@.new; then \
	    echo $@ is up-to-date; \
	    rm $@.new; \
	else \
	    echo updating $@; \
	    mv $@.new $@; \
	fi
