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

@INTLTOOL_XML_NOMERGE_RULE@
gsettings_SCHEMAS = org.mate.maximus.gschema.xml
@GSETTINGS_RULES@

%.gschema.xml.in: %.gschema.xml.in.in Makefile
	$(AM_V_GEN) $(SED) -e 's^\@GETTEXT_PACKAGE\@^$(GETTEXT_PACKAGE)^g' < $< > $@

desktopdir=$(sysconfdir)/xdg/autostart
dist_desktop_DATA=mate-maximus-autostart.desktop

EXTRA_DIST =  $(schema_DATA)
