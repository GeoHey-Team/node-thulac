{
    "includes": [
        './common.gypi',
    ],
    'targets': [
        {
            'target_name': 'segmentor',
            'sources': [
                './src/bigram_model.h',
                './src/cb_decoder.h',
                './src/cb_model.h',
                './src/cb_ngram_feature.h',
                './src/cb_tagging_decoder.h',
                './src/chinese_charset.h',
                './src/dat.h',
                './src/filter.h',
                './src/negword.h',
                './src/postprocess.h',
                './src/preprocess.h',
                './src/punctuation.h',
                './src/thulac_base.h',
                './src/thulac_character.h',
                './src/thulac_raw.h',
                './src/timeword.h',
                './src/verbword.h',

                'segmentator.cc',
                'wrapper.hpp',
                'wrapper.cpp',
            ],
            'cflags_cc': ['-std=c++0x'],
            'include_dirs':
            [
                './src/',
                "<!(node -e \"require('nan')\")",
            ],
        },
    ],
}
