CCFLAGS=[
	"-ftrapv","-g","-O0", # DEBUG OPTIONS
	#"-O2","-D NDEBUG", # RELEASE OPTIONS
	# "-pg", # for gperf
	"-std=c++17","-I./src", "-Wall",
	#"-ferror-limit=3",   # clang
	"-fmax-errors=3",    # gcc
]
LINKFLAGS=["-g","-O0"]
env=Environment(CPPPATH=['./src'])
env.Append(BUILDERS={
	#'Embed': Builder(action='ld -r -b binary $SOURCE -o $TARGET'),
	#'DfPack': Builder(action='./dfpack < $SOURCE >')  rc,png->ss
	#'PixFont' ttf->rc,png
	#'Svg2Png' svg->png
})
[]
ext=[env.Object('./src/ext/ext.cpp', CCFLAGS=CCFLAGS)]
mat=[env.Object('./src/ext/mat.cpp', CCFLAGS=CCFLAGS)]
png=[env.Object('./src/ext/png.cpp', CCFLAGS=CCFLAGS), env.Object('./src/ext/lodepng/lodepng.cpp', CCFLAGS=CCFLAGS)]
df =[env.Object('./src/dfpack/df.cpp', CCFLAGS=CCFLAGS)]
parser=[env.Object('./src/dfpack/parser.cpp', CCFLAGS=CCFLAGS)]
env.Program('dfpack',
	ext+mat+png+df+parser+['./src/dfpack/dfpack.cpp'],
	CCFLAGS=CCFLAGS, LINKFLAGS=LINKFLAGS+[])
env.Program('dftest',
	ext+mat+png+df+['./src/dfpack/dftest.cpp'],
	CCFLAGS=CCFLAGS, LINKFLAGS=LINKFLAGS+[])
# convert aaa.svg.png  -set colorspace Gray aaa.svg.png
env.Program('moon', ext+mat+png+[
		#env.Embed('res/font.o', "res/font.ss"),
		#env.Embed('res/unit.o', "res/unit.ss"),
		#env.Embed('res/terr.o', "res/terr.ss"),
		'./src/moon/my.cpp',
		'./src/moon/ren.cpp',
		'./src/moon/moon.cpp',
	],
	CCFLAGS=CCFLAGS, LINKFLAGS=LINKFLAGS+['-lSDL2','-lGL','-lGLEW'])
env.Program('pixfont', png+['./src/pixfont/main.cpp',],
	CCFLAGS=CCFLAGS+[
		'-I/usr/include/freetype2',
		#'-I/usr/include/libpng12',
	],
	LINKFLAGS=LINKFLAGS+['-lfreetype'])
