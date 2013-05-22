{
'targets': [
	{
		'target_name': 'light2d',
		'target_arch': 'x64',
		'type': 'executable',
		'sources': [
			'src/main.cpp',
			'src/Light.h',
			'src/Light.cpp',
			'src/Geom.h',
			'src/Geom.cpp',
			'src/Vector.h',
			'src/Vector.cpp',
		],
		'include_dirs': [
			'include',
		],
		'conditions': [
			['OS=="mac"', {
				'ldflags': [
					'-Llib',
				],
				'libraries': [
					'Cocoa.framework',
					'IOKit.framework',
					'OpenGL.framework',
					'lib/libglfw.a',
				],
			}]
		],
		'xcode_settings': {
			"CLANG_CXX_LANGUAGE_STANDARD": "c++11",
			"CLANG_CXX_LIBRARY": "libc++",
			"NATIVE_ARCH": "x86_64",
		},
	}
]
}
