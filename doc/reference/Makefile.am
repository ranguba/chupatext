MAINTAINERCLEANFILE = Makefile.in

# The name of the module.
DOC_MODULE=chupatext

# The top-level SGML file.
DOC_MAIN_SGML_FILE=chupatext-docs.sgml

SOURCE_DIR_OPTIONS=					\
	--source-dir="$(top_srcdir)/chupatext"		\
	`test "$(top_builddir)" != "$(top_srcdir)" &&	\
	   echo --source-dir="$(top_builddir)/chupatext"`

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS =							\
	--deprecated-guards="CHUPATEXT_DISABLE_DEPRECATED"	\
	$(SOURCE_DIR_OPTIONS)

# Extra options to supply to gtkdoc-scangobj
SCANGOBJ_OPTIONS =					\
	--types="$(srcdir)/$(DOC_MODULE).types"

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR=../../chupatext

# Used for dependencies
HFILE_GLOB =								\
	$(top_builddir)/chupatext/chupa_version.h			\
	$(top_srcdir)/chupatext/chupa_*.h

#CFILE_GLOB=$(top_srcdir)/chupatext/*.c

# Headers to ignore
IGNORE_HFILES =

INCLUDES =					\
	-I$(top_srcdir)/chupatext		\
	-I$(top_srcdir)				\
	-I$(top_builddir)/chupatext		\
	$(GLIB_CFLAGS)				\
	$(GIO_UNIX_CFLAGS)			\
	$(GSF_CFLAGS)

GTKDOC_LIBS =						\
	$(top_builddir)/chupatext/libchupatext.la	\
	$(GLIB_LIBS)					\
	$(GSF_LIBS)					\
	$(GIO_UNIX_LIBS)

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS =						\
	--sgml-mode					\
	$(SOURCE_DIR_OPTIONS)

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
content_files =					\
	version.xml				\
	readme.xml				\
	news.xml				\
	chupatext.xml				\
	install-to-debian.xml			\
	install-to-ubuntu.xml			\
	install-to-others.xml

# Extra options to supply to gtkdoc-fixxref
FIXXREF_OPTIONS=

CATALOGS=ja.po

# include common portion ...
include $(top_srcdir)/gtk-doc.make

generated_xml_files =				\
	readme.xml				\
	readme.xml.ja				\
	news.xml				\
	news.xml.ja				\
	chupatext.xml				\
	install-to-debian.xml			\
	install-to-debian.xml.ja		\
	install-to-ubuntu.xml			\
	install-to-ubuntu.xml.ja		\
	install-to-others.xml			\
	install-to-others.xml.ja

CLEANFILES +=				\
	$(DOC_MODULE)-sections.txt	\
	$(generated_xml_files)

# Other files to distribute
EXTRA_DIST +=				\
	html-build.stamp		\
	version.xml.in			\
	$(generated_xml_files)		\
	overrides.txt			\
	ja.po

$(DOC_MODULE)-overrides.txt: $(srcdir)/overrides.txt
	cp $< $@


$(DOC_MODULE).pot: sgml-build.stamp
	xml2po -k -o $(DOC_MODULE).pot				\
	  $(srcdir)/$(DOC_MAIN_SGML_FILE) $(srcdir)/xml/*.xml

update-po: Makefile sgml.stamp
	$(MAKE) $(DOC_MODULE).pot
	for catalog in $(CATALOGS); do		\
	  xml2po -k -u $$catalog		\
	    $(srcdir)/$(DOC_MAIN_SGML_FILE)	\
	    $(srcdir)/xml/*.xml;		\
	done

RD2LIB_DIR = $(top_srcdir)/misc
RD2REFENTRY_LIB_FILE = rd2refentry-lib.rb
RD2REFENTRY_LIB = $(RD2LIB_DIR)/$(RD2REFENTRY_LIB_FILE)
RD2DOCBOOK = ruby -I $(RD2LIB_DIR) $(RD2) -r $(RD2REFENTRY_LIB_FILE) --name=CHUPATEXT
RD2DOCBOOK_JA = $(RD2DOCBOOK) --po=$(srcdir)/ja.po

news.xml: $(top_srcdir)/NEWS $(RD2REFENTRY_LIB) news.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/NEWS > news.xml

news.xml.ja: $(top_srcdir)/NEWS.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/NEWS.ja > news.xml.ja

readme.xml: $(top_srcdir)/README $(RD2REFENTRY_LIB) readme.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/README > readme.xml

readme.xml.ja: $(top_srcdir)/README.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/README.ja > readme.xml.ja

chupatext.xml: $(top_srcdir)/doc/chupatext.rd $(RD2REFENTRY_LIB) chupatext.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/chupatext.rd > chupatext.xml

chupatext.xml.ja: $(top_srcdir)/doc/chupatext.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/chupatext.rd.ja > chupatext.xml.ja

install-to-debian.xml: $(top_srcdir)/doc/install-to-debian.rd $(RD2REFENTRY_LIB) install-to-debian.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-debian.rd > install-to-debian.xml

install-to-debian.xml.ja: $(top_srcdir)/doc/install-to-debian.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-debian.rd.ja > install-to-debian.xml.ja

install-to-ubuntu.xml: $(top_srcdir)/doc/install-to-ubuntu.rd $(RD2REFENTRY_LIB) install-to-ubuntu.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-ubuntu.rd > install-to-ubuntu.xml

install-to-ubuntu.xml.ja: $(top_srcdir)/doc/install-to-ubuntu.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-ubuntu.rd.ja > install-to-ubuntu.xml.ja

install-to-others.xml: $(top_srcdir)/doc/install-to-others.rd $(RD2REFENTRY_LIB) install-to-others.xml.ja
	$(RD2DOCBOOK) $(top_srcdir)/doc/install-to-others.rd > install-to-others.xml

install-to-others.xml.ja: $(top_srcdir)/doc/install-to-others.rd.ja $(RD2REFENTRY_LIB) ja.po
	$(RD2DOCBOOK_JA) $(top_srcdir)/doc/install-to-others.rd.ja > install-to-others.xml.ja

upload: docs
	rm -rf tmp/
	mkdir -p tmp/
	cp -rp html/ tmp/reference
	for catalog in $(CATALOGS); do					\
	  lang=`echo $$catalog | sed 's/.po$$//'`;			\
	  cp -rp $$lang/html/ tmp/reference/$$lang/;			\
	done
	chmod -R g+w tmp/reference
	ruby $(top_srcdir)/misc/prepare-gtk-doc-html-for-publishing.rb	\
	  --template-directory=$(srcdir) --title "$(PACKAGE_TITLE)"	\
	  tmp/reference/*.html
	ruby $(top_srcdir)/misc/prepare-gtk-doc-html-for-publishing.rb	\
	  --template-directory=$(srcdir) --title "$(PACKAGE_TITLE)"	\
	  --language=ja tmp/reference/ja/*.html
	rsync -avz --delete tmp/reference/ $(RSYNC_PATH)/reference/
