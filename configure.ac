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

PKG_CHECK_MODULES(POPPLER_GLIB, poppler-glib, [have_poppler=yes], [:])
AM_CONDITIONAL([HAVE_POPPLER_GLIB], [test "$have_poppler" = "yes"])
PKG_CHECK_MODULES(WV, wv-1.0, [have_wv=yes], [:])
AM_CONDITIONAL([HAVE_WV], [test "$have_wv" = "yes"])

dnl **************************************************************
dnl Check for Cutter
dnl **************************************************************
m4_ifdef([AC_CHECK_GCUTTER], [
AC_CHECK_GCUTTER(>= 1.1.4)
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

AC_SUBST(build_includedir, '$(top_build_prefix)')
AC_SUBST(top_includedir, '$(top_srcdir)')

AC_CONFIG_FILES([Makefile
		 chupatext/Makefile
		 test/Makefile
		 module/Makefile])
AC_OUTPUT

