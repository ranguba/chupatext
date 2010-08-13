AC_PREREQ(2.59)

AC_INIT()

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])

AC_CONFIG_SRCDIR([include/chupatext.h])
AM_CONFIG_HEADER([include/chupatext/config.h])

AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
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

moduledir='$(pkglibdir)/module'
AC_SUBST(moduledir)

AC_SUBST(build_includedir, '$(top_build_prefix)include')
AC_SUBST(top_includedir, '$(top_srcdir)/include')

CHUPATEXT_MAJOR=`echo $CHUPATEXT_VERSION | sed '[s/[-.].*//]'`
CHUPATEXT_MINOR=`echo $CHUPATEXT_VERSION | sed '[s/^[^-.]*\.//;s/[-.].*//]'`
CHUPATEXT_MICRO=`echo $CHUPATEXT_VERSION | sed '[s/^[^-.]*\.[^-.]*\.//;s/[-.].*//]'`
AC_SUBST(CHUPATEXT_MAJOR)AC_SUBST(CHUPATEXT_MINOR)AC_SUBST(CHUPATEXT_MICRO)
AC_DEFINE_UNQUOTED(CHUPATEXT_VERSION_MAJOR, [$CHUPATEXT_MAJOR], [major])
AC_DEFINE_UNQUOTED(CHUPATEXT_VERSION_MINOR, [$CHUPATEXT_MINOR], [minor])
AC_DEFINE_UNQUOTED(CHUPATEXT_VERSION_MICRO, [$CHUPATEXT_MICRO], [micro])
AC_DEFINE_UNQUOTED(CHUPATEXT_VERSION, ["$CHUPATEXT_VERSION"], [version])

AC_CONFIG_FILES([Makefile
		 lib/Makefile
		 module/Makefile])
AC_OUTPUT

