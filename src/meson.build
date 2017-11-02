cc = meson.get_compiler('c')
m_dep = cc.find_library('m', required : false)

TP1_sources = [
  'CT29.c', 'tests.c',
]

executable('tp1', TP1_sources,
  dependencies: m_dep,
  install: true,
)
