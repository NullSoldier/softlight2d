{
'targets': 
[
	{
		'target_name': 'light2d',
		'type': 'executable',
		'sources': [
			'src/*.cpp"
		],
		'include_dirs': [
			'include'
		],
		'libraries': [
			'../lib',
		],
		['OS=="mac"', {
			'cflags': [
				'-std=c++11'
				'-stdlib=libC++'
			],
			'ldflags': {
				'-lglfw',
			},
			'link_settings': {
				'libraries': [
					'Cocoa.framework',
					'IOKit.framework',
					'OpenGL.framework'
				],
			}
		}],
	}
]
}
