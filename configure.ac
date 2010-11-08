AC_PREREQ(2.59)

m4_define([chupa_version_major], [0])
m4_define([chupa_version_minor], [5])
m4_define([chupa_version_micro], [0])
m4_define([chupa_version_full],
          [chupa_version_major.chupa_version_minor.chupa_version_micro])

AC_INIT(chupatext, [chupa_version_full], groonga-dev@lists.sourceforge.jp)

CHUPA_VERSION_MAJOR=chupa_version_major
CHUPA_VERSION_MINOR=chupa_version_minor
CHUPA_VERSION_MICRO=chupa_version_micro
CHUPA_VERSION=chupa_version_full
version_txt=$(dirname $0)/version.txt
if test -f "$version_txt"; then
  source "$version_txt"
else
  CHUPA_N_COMMITS=0
  CHUPA_COMMIT_ID=
  CHUPA_RELEASE_DATE=0
fi
AC_SUBST(CHUPA_VERSION_MAJOR)
AC_SUBST(CHUPA_VERSION_MINOR)
AC_SUBST(CHUPA_VERSION_MICRO)
AC_SUBST(CHUPA_VERSION)
AC_SUBST(CHUPA_N_COMMITS)
AC_SUBST(CHUPA_COMMIT_ID)
AC_SUBST(CHUPA_RELEASE_DATE)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([chupatext.h])
AM_CONFIG_HEADER([chupatext/config.h])

PROJECT_NAME="Ranguba"
AC_SUBST(PROJECT_NAME)

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

PACKAGE_TITLE=ChupaText
AC_SUBST(PACKAGE_TITLE)

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)

AC_PROG_LIBTOOL

LT_CURRENT=$CHUPA_VERSION_MAJOR
LT_REVISION=$CHUPA_VERSION_MINOR
LT_AGE=$CHUPA_VERSION_MICRO
LT_VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)
AC_SUBST(LT_VERSION_INFO)

LIBTOOL_EXPORT_OPTIONS='-export-symbols-regex "[^[Cc]hupa.*]"'
AC_SUBST(LIBTOOL_EXPORT_OPTIONS)

dnl IT_PROG_INTLTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

if test "x$GCC" = "xyes"; then
  case " $CFLAGS " in
  *[\ \	]-Wall[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wall" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-declarations[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-declarations" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wmissing-prototypes[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wmissing-prototypes" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wpointer-arith[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wpointer-arith" ;;
  esac

  case " $CFLAGS " in
  *[\ \	]-Wcast-align[\ \	]*) ;;
  *) CFLAGS="$CFLAGS -Wcast-align" ;;
  esac
fi

dnl **************************************************************
dnl Check for GLib
dnl **************************************************************
m4_ifdef([AM_PATH_GLIB_2_0], [
  GLIB_REQUIRED=2.24.0
  AC_SUBST(GLIB_REQUIRED)
  AM_PATH_GLIB_2_0($GLIB_REQUIRED,
		   [ac_glib_available="yes"],
		   [ac_glib_available="no"],
		   [gobject gio])
  ],
  [ac_glib_available="no"])
AC_MSG_CHECKING(for glib availablity)
AC_MSG_RESULT($ac_glib_available)
if test "$ac_glib_available" != "yes"; then
    AC_MSG_ERROR([no glib available])
fi

dnl **************************************************************
dnl Check for GetText
dnl **************************************************************
AC_PROG_INTLTOOL([0.35.0])
AM_GLIB_GNU_GETTEXT
GETTEXT_PACKAGE=chupatext
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"], [Gettext package])

dnl **************************************************************
dnl Check for Packages
dnl **************************************************************
PKG_CHECK_MODULES(GSF, libgsf-1, [have_gsf=yes], [have_gsf=no])
AM_CONDITIONAL([HAVE_GSF], [test "$have_gsf" = "yes"])
PKG_CHECK_MODULES(GIO_UNIX, gio-unix-2.0,
                  [have_gio_unix=yes], [have_gio_unix=no])
AM_CONDITIONAL([HAVE_GIO_UNIX], [test "$have_gio_unix" = "yes"])
PKG_CHECK_MODULES(POPPLER_GLIB, poppler-glib,
                  [have_poppler=yes], [have_poppler=no])
AM_CONDITIONAL([HAVE_POPPLER_GLIB], [test "$have_poppler" = "yes"])
PKG_CHECK_MODULES(WV, wv-1.0, [have_wv=yes], [have_wv=no])
AM_CONDITIONAL([HAVE_WV], [test "$have_wv" = "yes"])
PKG_CHECK_MODULES(GTK, gtk+-2.0, [have_gtk=yes], [have_gtk=no])
PKG_CHECK_MODULES(GOFFICE, libgoffice-0.8, [have_goffice=yes], [have_goffice=no])
AM_CONDITIONAL([HAVE_GOFFICE], [test "$have_goffice" = "yes"])

dnl **************************************************************
dnl Check for Ruby
dnl **************************************************************
ruby_available="no"
AC_ARG_WITH([ruby],
            AS_HELP_STRING([--with-ruby=PATH],
                           [Ruby interpreter path (default: auto-detect)]),
            [RUBY="$withval"])

if test "$RUBY" != "no"; then
  AC_PATH_PROG(RUBY, ruby, none)
  if test "$RUBY" != "none"; then
    ruby_available="yes"
  fi
fi
AC_MSG_CHECKING([for Ruby availability])
AC_MSG_RESULT($ruby_available)

RBCONFIG="$RUBY -rrbconfig"dnl
AC_DEFUN([RUBY_CONFIG],dnl
[dnl
AS_VAR_PUSHDEF([rbcv], m4_ifval([$2], [$2],
				m4_bmatch([$1],
					  [ruby], [$1], [RUBY], [$1],
					  [^[a-z]], [ruby]$1,
					  [RUBY_]$1)))dnl
if test $ruby_available = yes; then
    AC_MSG_CHECKING([for RbConfig[[$1]]])
    rbcv=`[$RBCONFIG] m4_ifval([$3], [-e ['%w{$3}.each{|k|k,v=k.split(/=/,2);RbConfig::MAKEFILE_CONFIG[k]=v}']])\
            [-e 'puts RbConfig.expand(RbConfig::MAKEFILE_CONFIG["$1"]||abort, RbConfig::MAKEFILE_CONFIG).strip'] 2>&1` || \
      ruby_available=no
    AC_MSG_RESULT($rbcv)
    if test $ruby_available = no; then
      AC_MSG_WARN(Ruby disabled)
    fi
fi dnl
AS_VAR_POPDEF([rbcv])dnl
])

RUBY_CONFIG(rubyhdrdir)
RUBY_CONFIG(arch)
RUBY_CONFIG(CFLAGS, [], [ARCH_FLAG=])
RUBY_CONFIG(CPPFLAGS)
RUBY_CONFIG(LDFLAGS, [], [ARCH_FLAG=])
RUBY_CONFIG(DLDFLAGS)
RUBY_CONFIG(LIBS)
RUBY_CONFIG(LIBRUBYARG)
RUBY_CONFIG(ARCH_FLAG)
RUBY_CPPFLAGS="$RUBY_CPPFLAGS -I$rubyhdrdir/$rubyarch -I$rubyhdrdir"
RUBY_LDFLAGS="$LIBRUBYARG $RUBY_LDFLAGS $RUBY_DLDFLAGS"
AC_SUBST(RUBY_CFLAGS)
AC_SUBST(RUBY_CPPFLAGS)
AC_SUBST(RUBY_LDFLAGS)
AC_SUBST(RUBY_LIBS)
AC_SUBST(RUBY_ARCH_FLAG)
AC_SUBST(rubyarch)

if test "$ruby_available" = "yes"; then
  AC_MSG_CHECKING([for Ruby/GLib2 CFLAGS])
  RUBY_GLIB2_CFLAGS=$($RUBY -e '
    ruby_glib2_gem = Gem.cache.find_name("glib2").first
    if ruby_glib2_gem.nil?
      begin
        require "glib2"
        ruby_glib2_path = $".grep(/glib2\.(?!rb\z)\w+\z/).first
        exit(false) if ruby_glib2_path.nil?
        print("-I", File.dirname(ruby_glib2_path))
        exit(true)
      rescue LoadError
        exit(false)
      end
    end
    ruby_glib2_path = ruby_glib2_gem.full_gem_path
    print("-I", File.join(ruby_glib2_path, "ext", "glib2"))')
  if test $? -ne 0; then
    ac_ruby_available="no"
  fi
  AC_MSG_RESULT($RUBY_GLIB2_CFLAGS)
  if test "$ac_ruby_available" = "no"; then
    AC_MSG_ERROR([no Ruby/GLib2 available])
  fi
  AC_SUBST(RUBY_GLIB2_CFLAGS)
fi

AM_CONDITIONAL([HAVE_RUBY], [test "$ruby_available" = "yes"])

dnl **************************************************************
dnl Checks for RD2
dnl **************************************************************

AC_PATH_PROG(RD2, rd2, rd2-not-found)
AM_CONDITIONAL([HAVE_RD2], [test "$RD2" != "rd2-not-found"])

dnl **************************************************************
dnl Checks for gtk-doc
dnl **************************************************************

GTK_DOC_CHECK([1.8])

dnl **************************************************************
dnl Check for Cutter
dnl **************************************************************
m4_ifdef([AC_CHECK_GCUTTER], [
AC_CHECK_GCUTTER(>= 1.1.5)
],
[ac_cv_use_cutter="no"])

AM_CONDITIONAL([WITH_CUTTER], [test "$ac_cv_use_cutter" != "no"])
if test "$ac_cv_use_cutter" != "no"; then
  AC_DEFINE(WITH_CUTTER, 1, [Define to 1 if you use Cutter])
fi

m4_ifdef([AC_CHECK_COVERAGE], [AC_CHECK_COVERAGE])
GENHTML_OPTIONS="--title 'chupatext Code Coverage'"


# Check for RubyForge
RUBYFORGE_PROJECT_NAME="groonga"
AC_SUBST(RUBYFORGE_PROJECT_NAME)

AC_ARG_WITH([rubyforge-user],
            AS_HELP_STRING([--with-rubyforge-user=USER],
                           [User name on RubyFroge (default: $USER)]),
            [RUBYFORGE_USER="$withval"],
            [RUBYFORGE_USER="$USER"])
AC_SUBST(RUBYFORGE_USER)

RUBYFORGE_PATH="/var/www/gforge-projects/\$(RUBYFORGE_PROJECT_NAME)"
AC_SUBST(RUBYFORGE_PATH)

RSYNC_PATH="\$(RUBYFORGE_USER)@rubyforge.org:\$(RUBYFORGE_PATH)"
AC_SUBST(RSYNC_PATH)


modulesdir='$(pkglibdir)/modules'
AC_SUBST(modulesdir)

decomposersdir='$(modulesdir)/decomposers'
AC_SUBST(decomposersdir)

ruby_libdir='$(pkglibdir)/ruby'
AC_SUBST(ruby_libdir)

ruby_lib_chupatextdir='$(ruby_libdir)/chupatext'
AC_SUBST(ruby_lib_chupatextdir)

ruby_decomposersdir='$(ruby_lib_chupatextdir)/decomposers'
AC_SUBST(ruby_decomposersdir)

descriptionsdir='$(sysconfdir)/$(PACKAGE)/descriptions'
AC_SUBST(descriptionsdir)

AC_SUBST(build_includedir, '$(top_build_prefix)')
AC_SUBST(top_includedir, '$(top_srcdir)')

AC_CONFIG_FILES([Makefile
		 chupatext.pc
		 chupatext-excel.pc
		 chupatext-pdf.pc
		 chupatext-ruby.pc
		 chupatext-tar.pc
		 chupatext-text.pc
		 chupatext-word.pc
		 chupatext-zip.pc
		 chupatext-gzip.pc
		 po/Makefile.in
		 build/Makefile
		 data/Makefile
		 data/descriptions/Makefile
		 chupatext/Makefile
		 chupatext/chupa_version.h
		 test/Makefile
		 test/fixtures/Makefile
		 test/ruby/Makefile
		 modules/Makefile
		 modules/excel/Makefile
		 modules/ruby/Makefile
		 modules/ruby/lib/Makefile
		 modules/ruby/lib/chupatext/Makefile
		 modules/ruby/lib/chupatext/decomposers/Makefile
		 license/Makefile
		 misc/Makefile
		 doc/Makefile
		 doc/reference/Makefile
		 doc/reference/version.xml
		 apt/Makefile])
AC_OUTPUT

echo
echo "Configure Result:"
echo
echo " Options:"
echo "  coverage                         : $cutter_enable_coverage"
echo
echo " Libraries:"
echo "  GLib                             : $ac_glib_available"
echo "  GSF                              : $have_gsf"
echo "  GIO Unix                         : $have_gio_unix"
echo "  Poppler                          : $have_poppler"
echo "  wvWare                           : $have_wv"
echo "  GTK+                             : $have_gtk"
echo "  GOffice                          : $have_goffice"
echo "  Ruby                             : $ruby_available ($RUBY)"
echo
echo " Paths:"
echo "  modules directory                : $modulesdir"
echo "  decomposers directory            : $decomposersdir"
echo "  Ruby decomposers directory       : $ruby_decomposersdir"
echo "  descriptions directory           : $descriptionsdir"
echo
echo " Tools:"
echo "  GTK-Doc                          : $enable_gtk_doc"
echo "  rd2                              : $RD2"
if test "$cutter_enable_coverage" = "yes"; then
echo "  lcov                             : $LCOV"
echo "  genhtml                          : $GENHTML"
fi
