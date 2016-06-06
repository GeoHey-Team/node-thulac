{
    'target_defaults': {
        'includes': [
            'common_conditions.gypi',
        ],

        'cflags': [
            '-fPIC',
        ],
        'cflags_cc!': ['-fno-rtti', '-fno-exceptions', ],
    },  # end 'target_defaults'
}
