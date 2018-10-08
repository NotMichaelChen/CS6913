project('program', 'c', default_options : ['c_std=c11', 'warning_level=3'])

add_global_arguments('-I../src', language : 'c')

cxx = meson.get_compiler('c')

src = [
	'src/main.c',
]

executable('out', src)
