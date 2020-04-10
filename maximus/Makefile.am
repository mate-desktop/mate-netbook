bin_PROGRAMS=mate-maximus

PKGDATADIR = $(datadir)/mate-maximus
DATADIR = $(datadir)
SYSCONFDIR = $(sysconfdir)
AM_CFLAGS = \
	$(MAXIMUS_DEPS_CFLAGS) \
	$(GCC_FLAGS) \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/tidy \
	-D_GNU_SOURCE \
	-DPKGDATADIR=\"$(PKGDATADIR)\" \
	-DDATADIR=\"$(DATADIR)\" \
	-DSYSCONFDIR=\"$(SYSCONFDIR)\" \
	$(WARN_CFLAGS)

mate_maximus_LDADD  = \
	$(MAXIMUS_DEPS_LIBS)

mate_maximus_SOURCES = \
	main.c \
	eggaccelerators.c \
	eggaccelerators.h \
	maximus-app.c \
	maximus-app.h \
	maximus-bind.c \
	maximus-bind.h \
	tomboykeybinder.c \
	tomboykeybinder.h \
	xutils.c \
	xutils.h 

gsettingsschema_in_files = org.mate.maximus.gschema.xml.in

gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

desktopdir=$(sysconfdir)/xdg/autostart
dist_desktop_DATA=mate-maximus-autostart.desktop

man_MANS = mate-maximus.1

CLEANFILES = $(gsettings_SCHEMAS) *.gschema.valid

EXTRA_DIST =  $(gsettingsschema_in_files) $(man_MANS)
