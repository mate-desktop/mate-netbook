bin_PROGRAMS=mate-maximus

PKGDATADIR = $(datadir)/mate-maximus
DATADIR = $(datadir)
SYSCONFDIR = $(sysconfdir)
AM_CFLAGS = \
	$(MAXIMUS_DEPS_CFLAGS) 				\
	$(GCC_FLAGS) 				\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/tidy \
	-D_GNU_SOURCE 				\
	-DPKGDATADIR=\"$(PKGDATADIR)\" \
	-DDATADIR=\"$(DATADIR)\" \
	-DSYSCONFDIR=\"$(SYSCONFDIR)\"

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

schemadir = $(sysconfdir)/mateconf/schemas
schema_DATA = maximus.schemas

if MATECONF_SCHEMAS_INSTALL
install-data-local:
	-MATECONF_CONFIG_SOURCE=$(MATECONF_SCHEMA_CONFIG_SOURCE) \
	$(MATECONFTOOL) --makefile-install-rule $(schema_DATA)
endif

if MATECONF_SCHEMAS_INSTALL
uninstall-local:
	-MATECONF_CONFIG_SOURCE=$(MATECONF_SCHEMA_CONFIG_SOURCE) \
	$(MATECONFTOOL) --makefile-uninstall-rule $(schema_DATA)
endif

desktopdir=$(sysconfdir)/xdg/autostart
dist_desktop_DATA=mate-maximus-autostart.desktop

EXTRA_DIST =  $(schema_DATA)
