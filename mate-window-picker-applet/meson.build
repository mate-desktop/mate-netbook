po_dir = join_paths(meson.source_root(), 'po')

applet_sources = [
  'applet.c',
  'task-item.c',
  'task-list.c',
  'task-title.c',
]

executable('mate-window-picker-applet', applet_sources,
           include_directories: config_inc,
           dependencies: netbook_deps,
           install_dir: privexecdir,
           install: true)

# .service files

service_data = configuration_data()
service_data.set('LOCATION', join_paths(prefix, get_option('libexecdir'), 'mate-window-picker-applet'))

configure_file(
  input :  'org.mate.panel.applet.MateWindowPickerFactory.service.in',
  output : 'org.mate.panel.applet.MateWindowPickerFactory.service',
  configuration : service_data,
  install : true,
  install_dir : join_paths(datadir, 'dbus-1/services')
)

# .gschema file

schema_data = configuration_data()
schema_data.set('GETTEXT_PACKAGE', meson.project_name())
configure_file(
  input :  'org.mate.panel.applet.mate-window-picker-applet.gschema.xml.in',
  output : 'org.mate.panel.applet.mate-window-picker-applet.gschema.xml',
  configuration : schema_data,
  install : true,
  install_dir : join_paths(datadir, 'glib-2.0', 'schemas')
)

# .desktop file

desktop_data = configuration_data()
desktop_data.set('LOCATION', join_paths(prefix, get_option('libexecdir'), 'mate-window-picker-applet'))
desktop_in_file = configure_file(
  input :  'org.mate.panel.MateWindowPicker.mate-panel-applet.desktop.in.in',
  output : 'org.mate.panel.MateWindowPicker.mate-panel-applet.desktop.in',
  configuration : desktop_data
)

i18n.merge_file(
  input :   desktop_in_file,
  output : 'org.mate.panel.MateWindowPicker.mate-panel-applet.desktop',
  type : 'desktop',
  po_dir : po_dir,
  install : true,
  install_dir : join_paths(matedir, 'applets')
)

install_data('mate-window-picker-applet-menu.xml',
  install_dir : join_paths(matedir, 'ui')
)
