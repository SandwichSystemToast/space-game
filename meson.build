project(
  'space-game',
  'c',
  default_options: [
    'default_library=static',
    'c_std=c2x',
    'warning_level=everything',
  ],
  subproject_dir: 'external'
)

src = []
dep = []
inc = []
arg = []

arg += '-Wno-unsafe-buffer-usage'
arg += '-Wno-unused-macros'
arg += '-Wno-reserved-macro-identifier'
arg += '-Wno-declaration-after-statement'
arg += '-Wno-missing-variable-declarations'

cmake = import('cmake')
raylib = cmake.subproject('raylib')
dep += raylib.dependency('raylib')

cc = meson.get_compiler('c')
dep += cc.find_library('m')

subdir('src')

executable(
  'space-game',
  src,
  dependencies : dep,
  include_directories : inc,
  cpp_args : arg,
  install : true
)
