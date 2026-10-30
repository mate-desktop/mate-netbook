if ENABLE_IN_PROCESS
pkglib_LTLIBRARIES = libmate-window-picker-applet.la
else
libexec_PROGRAMS=mate-window-picker-applet
endif

PKGDATADIR = $(datadir)/mate-window-picker-applet
AM_CFLAGS=\
	$(MATEWINDOWPICKER_DEPS_CFLAGS) \
	$(GCC_FLAGS) \
	-I$(top_builddir) \
	-I$(top_srcdir) \
	-D_GNU_SOURCE \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	-DPKGDATADIR=\"$(PKGDATADIR)\" \
	-DMATEWINDOWPICKER_MENU_UI_DIR=\""$(xmluidir)"\" \
	$(WARN_CFLAGS)

if ENABLE_IN_PROCESS

libmate_window_picker_applet_la_CFLAGS = $(AM_CFLAGS)

libmate_window_picker_applet_la_LIBADD = \
	$(MATEWINDOWPICKER_DEPS_LIBS) \
	-lm

libmate_window_picker_applet_la_SOURCES = \
	applet.c \
	task-item.c \
	task-item.h \
	task-list.c \
	task-list.h \
	task-title.c \
	task-title.h 

else

mate_window_picker_applet_CFLAGS = $(AM_CFLAGS)

mate_window_picker_applet_LDADD = \
	$(MATEWINDOWPICKER_DEPS_LIBS) \
	-lm

mate_window_picker_applet_SOURCES = \
	applet.c \
	task-item.c \
	task-item.h \
	task-list.c \
	task-list.h \
	task-title.c \
	task-title.h 

endif

appletdir       = $(datadir)/mate-panel/applets
applet_in_files = org.mate.panel.MateWindowPicker.mate-panel-applet.desktop.in
applet_DATA     = $(applet_in_files:.mate-panel-applet.desktop.in=.mate-panel-applet)

$(applet_DATA): $(applet_in_files) Makefile
	$(AM_V_GEN) $(MSGFMT) --desktop --keyword=Name --keyword=Description --template $< -d $(top_srcdir)/po -o $@

if !ENABLE_IN_PROCESS
servicedir       = $(datadir)/dbus-1/services
service_in_files = org.mate.panel.applet.MateWindowPickerFactory.service.in
service_DATA     = $(service_in_files:.service.in=.service)

org.mate.panel.applet.MateWindowPickerFactory.service: $(service_in_files)
	$(AM_V_GEN) $(SED) \
	-e "s|\@LOCATION\@|$(APPLET_LOCATION)|" \
	$< > $@
endif

gsettingsschema_in_files = org.mate.panel.applet.mate-window-picker-applet.gschema.xml.in

gsettings_SCHEMAS = $(gsettingsschema_in_files:.xml.in=.xml)
@GSETTINGS_RULES@

xmluidir = $(datadir)/mate-panel/ui
xmlui_DATA = mate-window-picker-applet-menu.xml

EXTRA_DIST = \
	$(applet_in_files).in \
	$(service_in_files) \
	$(ui_DATA) \
	$(convert_DATA) \
	$(gsettingsschema_in_files) \
	$(xmlui_DATA)

CLEANFILES = \
	$(applet_DATA) \
	$(applet_in_files) \
	$(gsettings_SCHEMAS) \
	*.gschema.valid \
	$(service_DATA)

