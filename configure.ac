AC_PREREQ(2.53)
AC_INIT(mate-netbook, 1.19.0, [])
AM_INIT_AUTOMAKE([1.9 foreign tar-ustar no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR(maximus/main.c)
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

# GLib min/max required versions
AC_DEFINE([GLIB_VERSION_MAX_ALLOWED], [GLIB_VERSION_2_36],
	[Warn on use of APIs added after GLib 2.36])
AC_DEFINE([GLIB_VERSION_MIN_REQUIRED], [GLIB_VERSION_2_36],
	[Warn on use of APIs deprecated before GLib 2.36])

dnl ==========================================================================
dnl i18n
dnl ==========================================================================

GETTEXT_PACKAGE=mate-netbook
AC_SUBST(GETTEXT_PACKAGE)
AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/po/LINGUAS'])
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the Gettext package name])

IT_PROG_INTLTOOL([0.50.1])

AM_GLIB_GNU_GETTEXT

#if test "x$GCC" = "xyes"; then
#        GCC_FLAGS="-g -Wall -Werror"
#fi

AC_SUBST(GCC_FLAGS)

GLIB_REQUIRED=2.36.0

PKG_CHECK_MODULES(MAXIMUS_DEPS, 
                  gtk+-3.0
                  glib-2.0 >= $GLIB_REQUIRED
                  gio-2.0 
                  libwnck-3.0
                  xtst
                  libfakekey
                  )
AC_SUBST(MAXIMUS_DEPS_CFLAGS)
AC_SUBST(MAXIMUS_DEPS_LIBS)

PKG_CHECK_MODULES(MATEWINDOWPICKER_DEPS, 
                  libmatepanelapplet-4.0 >= 1.17.0
                  gtk+-3.0
                  glib-2.0 >= $GLIB_REQUIRED
                  gio-2.0 
                  libwnck-3.0
                  )
AC_SUBST(MATEWINDOWPICKER_DEPS_CFLAGS)
AC_SUBST(MATEWINDOWPICKER_DEPS_LIBS)

GLIB_GSETTINGS

AC_OUTPUT([
Makefile
maximus/Makefile
maximus/org.mate.maximus.gschema.xml
mate-window-picker-applet/Makefile
mate-window-picker-applet/pixmaps/Makefile
mate-window-picker-applet/org.mate.panel.applet.mate-window-picker-applet.gschema.xml
po/Makefile.in
])
