AC_PREREQ(2.53)
AC_INIT(mate-netbook, 1.26.0, [])
AM_INIT_AUTOMAKE([1.9 foreign tar-ustar no-dist-gzip dist-xz check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_CONFIG_SRCDIR(maximus/main.c)
AC_CONFIG_MACRO_DIR([m4])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

MATE_COMPILE_WARNINGS

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL

dnl ==========================================================================
dnl i18n
dnl ==========================================================================
AM_GNU_GETTEXT_VERSION([0.19.8])
AM_GNU_GETTEXT([external])

GETTEXT_PACKAGE=mate-netbook
AC_SUBST(GETTEXT_PACKAGE)
AC_SUBST([CONFIG_STATUS_DEPENDENCIES],['$(top_srcdir)/po/LINGUAS'])
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the Gettext package name])

#if test "x$GCC" = "xyes"; then
#        GCC_FLAGS="-g -Wall -Werror"
#fi

AC_SUBST(GCC_FLAGS)

GLIB_REQUIRED=2.50.0
GTK_REQUIRED=3.22.0

PKG_CHECK_MODULES(MAXIMUS_DEPS, 
                  gtk+-3.0 >= $GTK_REQUIRED
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
                  gtk+-3.0 >= $GTK_REQUIRED
                  glib-2.0 >= $GLIB_REQUIRED
                  gio-2.0 
                  libwnck-3.0
                  )
AC_SUBST(MATEWINDOWPICKER_DEPS_CFLAGS)
AC_SUBST(MATEWINDOWPICKER_DEPS_LIBS)

GLIB_GSETTINGS

AC_CONFIG_FILES([
Makefile
maximus/Makefile
maximus/org.mate.maximus.gschema.xml
mate-window-picker-applet/Makefile
mate-window-picker-applet/org.mate.panel.applet.mate-window-picker-applet.gschema.xml
po/Makefile.in
])
AC_OUTPUT

dnl ---------------------------------------------------------------------------
dnl - Show summary
dnl ---------------------------------------------------------------------------

echo "
       mate-netbook $VERSION
       `echo mate-netbook $VERSION | sed "s/./=/g"`

       prefix:                        ${prefix}
       compiler:                      ${CC}
       cflags:                        ${CFLAGS}
       warning flags:                 ${WARN_CFLAGS}
"
