project(
  'space-game',
  'c',
  default_options: [
    'default_library=static',
    'c_std=gnu2x',
  ],
  subproject_dir: 'external'
)

src = []
dep = []
inc = []
arg = []

arg += '-Wno-unsafe-buffer-usage'
arg += '-Wno-unsafe-buffer-usage'
arg += '-Wno-unused-macros'
arg += '-Wno-reserved-macro-identifier'
arg += '-Wno-declaration-after-statement'
arg += '-Wno-missing-variable-declarations'
#arg += '-Wno-pre-c2x-compat'
#arg += '-Wno-padded'

cmake = import('cmake')
raylib = cmake.subproject('raylib')
raylib_opt = cmake.subproject_options()
raylib_opt.set_override_option('c_std', 'c99')
dep += raylib.dependency('raylib')

inc += include_directories('external')

cc = meson.get_compiler('c')
dep += cc.find_library('m')

subdir('src')

flecs_proj = subproject('flecs')
flecs_dep = flecs_proj.get_variable('flecs_dep')
dep += flecs_dep

executable(
  'space-game',
  src,
  dependencies : dep,
  include_directories : inc,
  c_args : arg,
  install : true
)
