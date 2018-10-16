project('program', 'c', default_options : ['c_std=c11', 'warning_level=3'])

add_global_arguments('-I../src', language : 'c')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

src = [
	'src/michaellib/string.c',
	'src/michaellib/buffer.c',
	'src/reader/metareader.c',
	'src/reader/reader.c',
	'src/reader/dirreader.c',
	'src/intermfilegen/filegen.c',
	'src/intermfilegen/docparser.c',
	'src/mergesort/mergesort.c',
	'src/main.c',
]

executable('out', src, dependencies: m_dep)
