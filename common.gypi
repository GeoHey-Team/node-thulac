{
    'target_defaults': {
        'includes': [
            'common_conditions.gypi',
        ],
        'cflags': [
            '-fPIC',
        ],
        'cflags_cc!': ['-fno-rtti', '-fno-exceptions', ],
        'link_settings': {
            'conditions': [
                ['OS == "mac"', {
                    'libraries': ['libiconv.dylib', ],
                }],
                ['OS == "win"', {
                    'libraries': ['-llibiconv.lib', ],
                }],
                ['OS == "linux"', {
                    'libraries': ['-liconv', ],
                }],
            ],
        },
    },  # end 'target_defaults'
}
