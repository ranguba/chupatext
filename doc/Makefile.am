SUBDIRS = reference

EXTRA_DIST =					\
	chupatext.rd				\
	chupatext.rd.ja				\
	install-to-debian.rd			\
	install-to-debian.rd.ja			\
	install-to-ubuntu.rd			\
	install-to-ubuntu.rd.ja			\
	install-to-others.rd			\
	install-to-others.rd.ja

dist_man1_MANS =	\
	chupatext.man

ja_man1dir = $(mandir)/ja/man1

dist_ja_man1_mans =	\
	chupatext.jman

EXTRA_DIST += $(dist_ja_man1_mans)

jman: $(dist_ja_man1_mans)

if HAVE_RD2
install-data-local: jman
else
install-data-local:
endif
	test -z "$(DESTDIR)$(ja_man1dir)" || $(mkdir_p) "$(DESTDIR)$(ja_man1dir)"
	@for jman in $(dist_ja_man1_mans); do			\
	  test -f "$${jman}" || break;				\
	  jman_path=$(top_builddir)/doc/$${jman};		\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;	\
	  echo $(INSTALL_DATA) $${jman_path}			\
	    "$(DESTDIR)$(ja_man1dir)"/$${jman_file};		\
	  $(INSTALL_DATA) $${jman_path}				\
	    "$(DESTDIR)$(ja_man1dir)"/$${jman_file};		\
	done

uninstall-local:
	@for jman in $(dist_ja_man1_mans); do			\
	  jman_file=`echo $${jman} | sed -e s/jman$$/1/`;	\
	  rm -f "$(DESTDIR)$(ja_man1dir)"/$${jman_file};	\
	done

# dirty...
chupatext.jman: chupatext.rd.ja

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_ja_man1_mans)

SUFFIXES = .rd .man .jman

RD2LIB_DIR = $(top_srcdir)/misc
RD2MAN_LIB_FILE = rd2man-lib.rb
RD2MAN_LIB = $(RD2LIB_DIR)/$(RD2MAN_LIB_FILE)
RD2MAN = ruby -I $(RD2LIB_DIR) $(RD2) -r $(RD2MAN_LIB_FILE)

.rd.man:
	$(RD2MAN) $< > $@

.rd.jman: # dirty...
	$(RD2MAN) $<.ja > $@
