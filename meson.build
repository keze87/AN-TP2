project('TP2', 'c',
  version: '0.1.0',
  meson_version: '>= 0.36.0',
  default_options : ['c_std=c11', 'cpp_std=c++11']
)

add_global_arguments([
  '-Wall',
  '-pedantic',
  '-pedantic-errors',
  '-I' + meson.build_root(),
], language: 'c')

subdir('src')
