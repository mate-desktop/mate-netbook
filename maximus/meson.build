man1_dir = join_paths(maximus_mandir, 'man1')

maximus_sources = [
  'eggaccelerators.c',
  'main.c',
  'maximus-app.c',
  'maximus-bind.c',
  'tomboykeybinder.c',
  'xutils.c'
]

executable('mate-maximus', maximus_sources,
           include_directories: config_inc,
           dependencies: netbook_deps,
           install: true)

# man files

install_data('mate-maximus.1',
  install_dir : man1_dir
)

# autostart file

install_data('mate-maximus-autostart.desktop',
install_dir: join_paths(get_option('prefix'), 'etc', 'xdg', 'autostart')
)

# .gschema file

schema_data = configuration_data()
schema_data.set('GETTEXT_PACKAGE', meson.project_name())
configure_file(
  input :  'org.mate.maximus.gschema.xml.in',
  output : 'org.mate.maximus.gschema.xml',
  configuration : schema_data,
  install : true,
  install_dir : join_paths(datadir, 'glib-2.0', 'schemas')
)
