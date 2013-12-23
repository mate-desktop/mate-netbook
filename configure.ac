AC_PREREQ(2.53)
AC_INIT(mate-netbook, 1.6.0, [])
AM_INIT_AUTOMAKE()
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


PKG_CHECK_MODULES(MAXIMUS_DEPS, 
                  gtk+-2.0
                  gio-2.0 
                  libmatewnck
                  xtst
                  libfakekey
                  unique-1.0
                  )
AC_SUBST(MAXIMUS_DEPS_CFLAGS)
AC_SUBST(MAXIMUS_DEPS_LIBS)

PKG_CHECK_MODULES(MATEWINDOWPICKER_DEPS, 
                  libmatepanelapplet-4.0
                  gtk+-2.0
                  glib-2.0
                  gio-2.0 
                  libmatewnck
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
