project('program', 'c', default_options : ['c_std=c11', 'warning_level=3'])

add_global_arguments('-I../src', language : 'c')

cxx = meson.get_compiler('c')

src = [
	'src/michaellib/string.c',
	'src/michaellib/buffer.c',
	'src/reader/metareader.c',
	'src/reader/reader.c',
	'src/reader/dirreader.c',
	'src/intermfilegen/filegen.c',
	'src/indexer/docwriter.c',
	'src/indexer/indexbuilder.c',
	'src/main.c',
]

executable('out', src)
