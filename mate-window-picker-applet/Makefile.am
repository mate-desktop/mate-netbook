libexec_PROGRAMS=mate-window-picker-applet

PKGDATADIR = $(datadir)/mate-window-picker-applet
AM_CFLAGS=\
	$(MATEWINDOWPICKER_DEPS_CFLAGS) 		\
	$(GCC_FLAGS) 				\
	-I$(top_builddir)			\
	-I$(top_srcdir)				\
	-D_GNU_SOURCE 				\
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	-DPKGDATADIR=\"$(PKGDATADIR)\" \
	-DMATEWINDOWPICKER_MENU_UI_DIR=\""$(xmluidir)"\"

mate_window_picker_applet_LDADD = \
	$(MATEWINDOWPICKER_DEPS_LIBS)

mate_window_picker_applet_SOURCES = \
	applet.c \
	task-item.c \
	task-item.h \
	task-list.c \
	task-list.h \
	task-title.c \
	task-title.h 

appletdir       = $(datadir)/mate-panel/applets
applet_in_files = org.mate.panel.MateWindowPicker.mate-panel-applet.in
applet_DATA     = $(applet_in_files:.mate-panel-applet.in=.mate-panel-applet)

$(applet_in_files): $(applet_in_files).in Makefile
	$(AM_V_GEN)sed \
	-e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	$< > $@

org.mate.panel.MateWindowPicker.mate-panel-applet: org.mate.panel.MateWindowPicker.mate-panel-applet.in $(INTLTOOL_MERGE) $(wildcard $(top_srcdir)/po/*po) ; $(INTLTOOL_MERGE) $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

APPLET_LOCATION=$(libexecdir)/mate-window-picker-applet

servicedir       = $(datadir)/dbus-1/services
service_in_files = org.mate.panel.applet.MateWindowPickerFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.mate.panel.applet.MateWindowPickerFactory.service: $(service_in_files)
	$(AM_V_GEN)sed \
	-e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	$< > $@

schemasdir = @MATECONF_SCHEMA_FILE_DIR@
schemas_in_files = mate-window-picker-applet.schemas.in
schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)
@INTLTOOL_SCHEMAS_RULE@

xmluidir = $(datadir)/mate-panel/ui
xmlui_DATA = mate-window-picker-applet-menu.xml

EXTRA_DIST =                                            \
	org.mate.panel.MateWindowPicker.panel-applet.in.in            \
	$(service_in_files)                             \
	$(ui_DATA)                                      \
	$(schemas_in_files)

CLEANFILES = $(applet_DATA) $(applet_DATA).in $(service_DATA) $(schemas_DATA)

clean:
	rm -f $(desktop_DATA) $(desktop_DATA).in $(server_DATA) $(server_DATA).in
