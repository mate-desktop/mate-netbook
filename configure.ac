AC_PREREQ(2.53)
AC_INIT(mate-netbook, 1.27.0, [])
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


AC_ARG_ENABLE([in-process],
              [AS_HELP_STRING([--enable-in-process],
                              [Build Window Picker Applet in-process])],
              [enable_in_process=$enableval],
              [enable_in_process=no])

# Automake conditional on whether to build in-process
AM_CONDITIONAL([ENABLE_IN_PROCESS], [test "x$enable_in_process" = "xyes"])
# Automake value expected to be substitued in .mate-panel-apple.* for the value of "InProcess"
AS_IF([test "x$enable_in_process" = "xyes"],
      [AC_SUBST([ENABLE_IN_PROCESS], [true])],
      [AC_SUBST([ENABLE_IN_PROCESS], [false])])
# C conditional on whether to build in-process
AS_IF([test "x$enable_in_process" = "xyes"],
      [AC_DEFINE([ENABLE_IN_PROCESS], [1], [building in-process])])

# Convenience C define selecting the right applet factory
AS_IF([test "x$enable_in_process" = "xyes"],
      [factory=MATE_PANEL_APPLET_IN_PROCESS_FACTORY],
      [factory=MATE_PANEL_APPLET_OUT_PROCESS_FACTORY])
AC_DEFINE_UNQUOTED([PANEL_APPLET_FACTORY], [$factory], [Panel applet factory])

# Get the applet location
AS_IF([test "x$enable_in_process" = "xyes"],
      [AS_AC_EXPAND(APPLET_LOCATION, $libdir/mate-netbook/libmate-window-picker-applet.so)],
      [AS_AC_EXPAND(APPLET_LOCATION, $libexecdir/mate-window-picker-applet)])

AS_AC_EXPAND([APPLET_LOCATION], [$APPLET_LOCATION])


GLIB_GSETTINGS

AC_OUTPUT([
Makefile
maximus/Makefile
maximus/org.mate.maximus.gschema.xml
mate-window-picker-applet/Makefile
mate-window-picker-applet/org.mate.panel.applet.mate-window-picker-applet.gschema.xml
mate-window-picker-applet/org.mate.panel.MateWindowPicker.mate-panel-applet.desktop.in
po/Makefile.in
])

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
       applet in-process:             ${enable_in_process}
"
