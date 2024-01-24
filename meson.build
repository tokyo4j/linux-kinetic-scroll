project(
  'demo',
  'c',
  default_options: [
    'c_std=gnu17',
    'warning_level=3',
  ],
)

pkg_dir = meson.global_source_root() / 'out'

shared_library(
  'native',
  ['src/native.c'],
  dependencies: [
    dependency('node-api-headers'),
    dependency('libinput'),
    dependency('libuv'),
  ],
  override_options: ['b_lundef=false'],
  name_prefix: '',
  name_suffix: 'so.node',
  install: true,
  install_dir: pkg_dir,
)

install_data('src/extension.js', install_dir: pkg_dir)
