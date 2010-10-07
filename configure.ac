AC_PREREQ(2.59)

AC_INIT(chupatext, 0)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([chupatext.h])
AM_CONFIG_HEADER([chupatext/config.h])

version_h="${srcdir}/chupatext/version.h"
VERSION=`sed '[/#define CHUPA_VERSION_STRING /!d;s///;s/"//g]' $version_h`

AM_INIT_AUTOMAKE($PACKAGE_NAME, $VERSION)
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])])

AC_ENABLE_SHARED(yes)
AC_ENABLE_STATIC(no)

AC_PROG_LIBTOOL

dnl LT_CURRENT=1
dnl LT_REVISION=0
dnl LT_AGE=1
dnl LT_VERSION_INFO="$LT_CURRENT:$LT_REVISION:$LT_AGE"
dnl AC_SUBST(LT_CURRENT)
dnl AC_SUBST(LT_REVISION)
dnl AC_SUBST(LT_AGE)
dnl AC_SUBST(LT_VERSION_INFO)

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
PKG_CHECK_MODULES(GSF, libgsf-1, [have_gsf=yes], [:])
AM_CONDITIONAL([HAVE_GSF], [test "$have_gsf" = "yes"])
PKG_CHECK_MODULES(GIO_UNIX, gio-unix-2.0, [have_gio_unix=yes], [:])
AM_CONDITIONAL([HAVE_GIO_UNIX], [test "$have_gio_unix" = "yes"])
PKG_CHECK_MODULES(POPPLER_GLIB, poppler-glib, [have_poppler=yes], [:])
AM_CONDITIONAL([HAVE_POPPLER_GLIB], [test "$have_poppler" = "yes"])
PKG_CHECK_MODULES(WV, wv-1.0, [have_wv=yes], [:])
AM_CONDITIONAL([HAVE_WV], [test "$have_wv" = "yes"])
PKG_CHECK_MODULES(GOFFICE, libgoffice-0.8, [have_goffice=yes], [:])
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
AS_VAR_PUSHDEF([rbcv], m4_if([$2], [$2],
			     m4_bmatch([$1],
				       [ruby], [$1], [RUBY], [$1],
				       [^[a-z]], [ruby]$1,
				       [RUBY_]$1)))dnl
if test $ruby_available = yes; then
    AC_MSG_CHECKING([for RbConfig[[$1]]])
    rbcv=`[$RBCONFIG -e 'puts RbConfig::CONFIG["$1"].strip'] 2>&1` || \
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
RUBY_CONFIG(CFLAGS)
RUBY_CONFIG(CPPFLAGS)
RUBY_CONFIG(LDFLAGS)
RUBY_CONFIG(DLDFLAGS)
RUBY_CONFIG(LIBS)
RUBY_CONFIG(LIBRUBYARG)
RUBY_CPPFLAGS="$RUBY_CPPFLAGS -I$rubyhdrdir/$rubyarch -I$rubyhdrdir"
RUBY_LDFLAGS="$LIBRUBYARG $RUBY_LDFLAGS $RUBY_DLDFLAGS"
AC_SUBST(RUBY_CFLAGS)
AC_SUBST(RUBY_CPPFLAGS)
AC_SUBST(RUBY_LDFLAGS)
AC_SUBST(RUBY_LIBS)
AC_SUBST(rubyarch)

AM_CONDITIONAL([HAVE_RUBY], [test "$ruby_available" = "yes"])

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

moduledir='$(pkglibdir)/module'
AC_SUBST(moduledir)

decomposerdir='$(moduledir)/decomposer'
AC_SUBST(decomposerdir)

factorydir='$(moduledir)/factory'
AC_SUBST(factorydir)

descriptionsdir='$(sysconfdir)/$(PACKAGE)/descriptions'
AC_SUBST(descriptionsdir)

AC_SUBST(build_includedir, '$(top_build_prefix)')
AC_SUBST(top_includedir, '$(top_srcdir)')

AC_CONFIG_FILES([Makefile
		 po/Makefile.in
		 build/Makefile
		 tool/Makefile
		 data/Makefile
		 data/descriptions/Makefile
		 chupatext/Makefile
		 test/Makefile
		 test/fixtures/Makefile
		 module/Makefile
		 binding/Makefile
		 binding/ruby/Makefile
		 license/Makefile
		 apt/Makefile])
AC_OUTPUT

