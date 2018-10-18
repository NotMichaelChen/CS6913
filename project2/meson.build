project('program', 'c', default_options : ['c_std=c11', 'warning_level=3'])

add_global_arguments('-I../src', language : 'c')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

src = [
	'src/michaellib/ulongvec.c',
	'src/michaellib/string.c',
	'src/michaellib/utility.c',
	'src/michaellib/postingvec.c',
	'src/reader/metareader.c',
	'src/reader/reader.c',
	'src/reader/dirreader.c',
	'src/indexbuilder/naive/indexer.c',
	'src/indexbuilder/lexicon.c',
	'src/mergesort/mergesort.c',
	'src/intermfilegen/docparser.c',
	'src/intermfilegen/filegen.c',
	'src/compression/varbyte.c',
	'src/posting.c',
	'src/main.c',
	'src/indexbuilder/pagetable.c',
]

executable('out', src, dependencies: m_dep)
