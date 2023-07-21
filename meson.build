project('mate-netbook', 'c',
        version: '1.27.0',
        meson_version: '>= 0.50.0',
        license: 'GPLv3.0+')

i18n = import('i18n')

prefix = get_option('prefix')
gettext_package = meson.project_name()
privexecdir = join_paths(prefix, get_option('libexecdir'))
datadir = join_paths(prefix, get_option('datadir'))
matedir = join_paths(prefix, get_option('datadir'), 'mate-panel')
maximus_mandir = join_paths(prefix, get_option('mandir'))

glib_req = '>= 2.50.0'
gtk_req = '>= 3.22.0'
libpanel_req = '>= 1.17.0'

cc = meson.get_compiler('c')

# config.h configuration data
cdata = configuration_data()
cdata.set_quoted('GETTEXT_PACKAGE', 'mate-netbook')
cdata.set_quoted('PACKAGE_VERSION', meson.project_version())
cdata.set_quoted('PACKAGE_URL', 'https://mate-desktop.org/')
cdata.set_quoted('SYSCONFDIR', join_paths(prefix, 'etc'))
cdata.set_quoted('MATELOCALEDIR', join_paths(prefix, get_option('localedir')))
cdata.set_quoted('MATEWINDOWPICKER_MENU_UI_DIR', join_paths(prefix, get_option('localedir'), 'mate-panel', 'ui'))

add_project_arguments('-DHAVE_CONFIG_H', language: 'c')
add_project_arguments('-O2', language: 'c')
add_project_arguments('-Wmissing-prototypes', language: 'c')
config_inc = include_directories('.')

gtk_dep = dependency('gtk+-3.0', version: gtk_req)
glib_dep = dependency('glib-2.0', version: glib_req)
libwnck_dep = dependency('libwnck-3.0')
xtst_dep = dependency('xtst')
libfakekey_dep = dependency('libfakekey')
libpanel_dep = dependency('libmatepanelapplet-4.0', version: libpanel_req)
m_dep = cc.find_library('m', required : false)

netbook_deps = [gtk_dep, glib_dep, libwnck_dep, xtst_dep, libfakekey_dep, libpanel_dep, m_dep]

subdir('mate-window-picker-applet')
subdir('po')
subdir('maximus')

# write config.h
configure_file(output: 'config.h', configuration: cdata)
# Summary

summary = [
  'configuration summary:',
  '',
  '           project:    @0@ @1@'.format(meson.project_name(), meson.project_version()),
  '            prefix:    @0@'.format(prefix),
  '          compiler:    @0@'.format('gcc'),
  '            cflags:    @0@'.format('-g -O2'),
  '     warning flags:    @0@'.format('-Wall -Wmissing-prototypes'),
  ''
]
message('\n'.join(summary))
