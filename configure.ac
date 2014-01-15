AC_PREREQ(2.53)
AC_INIT(mate-netbook, 1.6.0, [])
AM_INIT_AUTOMAKE([1.9 foreign tar-ustar no-dist-gzip dist-xz])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR(maximus/main.c)
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

dnl ==========================================================================
dnl i18n
dnl ==========================================================================

GETTEXT_PACKAGE=mate-netbook
AC_SUBST(GETTEXT_PACKAGE)
AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/po/LINGUAS'])
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the Gettext package name])

IT_PROG_INTLTOOL([0.34])

AM_GLIB_GNU_GETTEXT

#if test "x$GCC" = "xyes"; then
#        GCC_FLAGS="-g -Wall -Werror"
#fi

AC_SUBST(GCC_FLAGS)


AC_MSG_CHECKING([which gtk+ version to compile against])
AC_ARG_WITH([gtk],
  [AS_HELP_STRING([--with-gtk=2.0|3.0],[which gtk+ version to compile against (default: 2.0)])],
  [case "$with_gtk" in
     2.0|3.0) ;;
     *) AC_MSG_ERROR([invalid gtk version specified]) ;;
   esac],
  [with_gtk=2.0])
AC_MSG_RESULT([$with_gtk])

case "$with_gtk" in
  2.0) GTK_API_VERSION=2.0
       LIBWNCK_API_VERSION=1.0
       LIBUNIQUE_API_VERSION=1.0
       ;;
  3.0) GTK_API_VERSION=3.0
       LIBWNCK_API_VERSION=3.0
       LIBUNIQUE_API_VERSION=3.0
       ;;
esac

PKG_CHECK_MODULES(MAXIMUS_DEPS, 
                  gtk+-$GTK_API_VERSION
                  gio-2.0 
                  libwnck-$LIBWNCK_API_VERSION
                  xtst
                  libfakekey
                  unique-$LIBUNIQUE_API_VERSION
                  )
AC_SUBST(MAXIMUS_DEPS_CFLAGS)
AC_SUBST(MAXIMUS_DEPS_LIBS)

PKG_CHECK_MODULES(MATEWINDOWPICKER_DEPS, 
                  libmatepanelapplet-4.0
                  gtk+-$GTK_API_VERSION
                  glib-2.0
                  gio-2.0 
                  libwnck-$LIBWNCK_API_VERSION
                  )
AC_SUBST(MATEWINDOWPICKER_DEPS_CFLAGS)
AC_SUBST(MATEWINDOWPICKER_DEPS_LIBS)

GLIB_GSETTINGS

AC_OUTPUT([
Makefile
maximus/Makefile
mate-window-picker-applet/Makefile
po/Makefile.in
])

echo "
    GTK+ API version:           ${GTK_API_VERSION}
"
