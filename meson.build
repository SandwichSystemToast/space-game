project(
  'space-game',
  'c',
  default_options: [
    'default_library=static',
    'c_std=c2x',
  ],
  subproject_dir: 'external'
)

src = []
dep = []
inc = []
arg = []

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
