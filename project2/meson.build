project('program', 'c', default_options : ['c_std=c11', 'warning_level=3'])

add_global_arguments('-I../src', language : 'c')

cc = meson.get_compiler('c')

m_dep = cc.find_library('m', required : false)

src = [
	'src/reader/dirreader.c',
	'src/reader/metareader.c',
	'src/reader/reader.c',
	'src/main.c',
	'src/compression/varbyte.c',
	'src/indexbuilder/pagetable.c',
	'src/indexbuilder/lexicon.c',
	'src/indexbuilder/indexer.c',
	'src/michaellib/vector/bytevec.c',
	'src/michaellib/vector/stringvec.c',
	'src/michaellib/vector/ulongvec.c',
	'src/michaellib/utility.c',
	'src/michaellib/minheap.c',
	'src/michaellib/string.c',
	'src/mergesort/mergesort.c',
	'src/query/listpointer.c',
	'src/query/DAAT.c',
	'src/query/BM25.c',
	'src/intermfilegen/filegen.c',
	'src/intermfilegen/docparser.c',
	'src/posting.c',
]

executable('out', src, dependencies: m_dep)
