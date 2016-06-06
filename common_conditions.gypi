{
    'conditions': [
        ['OS == "win"',
         {
             'defines': [
                 '_CRT_SECURE_NO_WARNINGS',
                 'GR_GL_FUNCTION_TYPE=__stdcall',
             ],
             'msvs_disabled_warnings': [
                 4275,  # An exported class was derived from a class that was not exported
                 # This is an FYI about a behavior change from long ago. Chrome
                 # stifles it too.
                 4345,
                 # 'this' used in base member initializer list. Off by default in newer compilers.
                 4355,
             ],
             'msvs_cygwin_shell': 0,
             'msvs_settings': {
                 'VCCLCompilerTool': {
                     'WarningLevel': '3',
                     'ProgramDataBaseFileName': '$(OutDir)\\$(ProjectName).pdb',
                     'DebugInformationFormat': '3',
                     'ExceptionHandling': '0',
                     'AdditionalOptions': ['/MP', ],
                 },
                 'VCLinkerTool': {
                     'LargeAddressAware': 2,  # 2 means "Yes, please let me use more RAM on 32-bit builds."
                     'AdditionalDependencies': [
                         'OpenGL32.lib',
                         'usp10.lib',

                         # Prior to gyp r1584, the following were included
                         # automatically.
                         'kernel32.lib',
                         'gdi32.lib',
                         'winspool.lib',
                         'comdlg32.lib',
                         'advapi32.lib',
                         'shell32.lib',
                         'ole32.lib',
                         'oleaut32.lib',
                         'user32.lib',
                         'uuid.lib',
                         'odbc32.lib',
                         'odbccp32.lib',
                         'DelayImp.lib',
                     ],
                 },
             },
             'configurations': {
                 'Debug': {
                     'msvs_settings': {
                         'VCCLCompilerTool': {
                             # editAndContiue (/ZI)
                             'DebugInformationFormat': '4',
                             # optimizeDisabled (/Od)
                             'Optimization': '0',
                             'PreprocessorDefinitions': ['_DEBUG'],
                             # rtMultiThreadedDebugDLL (/MDd)
                             'RuntimeLibrary': '3',
                             'RuntimeTypeInfo': 'false',      # /GR-
                         },
                         'VCLinkerTool': {
                             'GenerateDebugInformation': 'true',  # /DEBUG
                             'LinkIncremental': '2',             # /INCREMENTAL
                         },
                     },
                 },
                 'Release': {
                     'msvs_settings': {
                         'VCCLCompilerTool': {
                             # programDatabase (/Zi)
                             'DebugInformationFormat': '3',
                             'Optimization': '3',                # full
                             # Changing the floating point model requires rebaseling gm images
                             # 'FloatingPointModel': '2',          # fast (/fp:fast)
                             'FavorSizeOrSpeed': '1',            # speed (/Ot)
                             'PreprocessorDefinitions': ['NDEBUG'],
                             # rtMultiThreadedDLL (/MD)
                             'RuntimeLibrary': '2',
                             'EnableEnhancedInstructionSet': '2',  # /arch:SSE2
                             'RuntimeTypeInfo': 'false',         # /GR-
                         },
                         'VCLinkerTool': {
                             'GenerateDebugInformation': 'true',  # /DEBUG
                         },
                     },
                 },
             },
             'conditions': [
                 ['"ninja" in "<!(echo %GYP_GENERATORS%)"', {
                  'configurations': {
                      'Debug_x64': {
                          'inherit_from': ['Debug'],
                          'msvs_settings': {
                              'VCCLCompilerTool': {
                                  # /ZI is not supported on 64bit
                                  # programDatabase (/Zi)
                                  'DebugInformationFormat': '3',
                              },
                          },
                      },
                      'Release_x64': {
                          'inherit_from': ['Release'],
                          'msvs_settings': {
                              'VCCLCompilerTool': {
                                  # Don't specify /arch. SSE2 is implied by 64bit and
                                  # specifying it warns.
                                  'EnableEnhancedInstructionSet': '0',
                              },
                          },
                      },
                      'Release_Developer_x64': {
                          'inherit_from': ['Release_Developer'],
                          'msvs_settings': {
                              'VCCLCompilerTool': {
                                  # Don't specify /arch. SSE2 is implied by 64bit and
                                  # specifying it warns.
                                  'EnableEnhancedInstructionSet': '0',
                              },
                          },
                      },
                  },
                  }],
                 ['arch_width == 64', {
                  'msvs_configuration_platform': 'x64',
                  }],
                 ['arch_width == 32', {
                  'msvs_configuration_platform': 'Win32',
                  }],
                 ['warnings_as_errors', {
                  'msvs_settings': {
                      'VCCLCompilerTool': {
                          'WarnAsError': 'true',
                          'AdditionalOptions': [
                              '/we4189',  # initialized but unused var warning
                          ],
                      },
                  },
                  }],
                 ['win_exceptions', {
                  'msvs_settings': {
                      'VCCLCompilerTool': {
                          'AdditionalOptions': [
                              '/EHsc',
                          ],
                      },
                  },
                  }],
                 ['win_ltcg', {
                  'configurations': {
                      'Release': {
                          'msvs_settings': {
                              'VCCLCompilerTool': {
                                  'WholeProgramOptimization': 'true',  # /GL
                              },
                              'VCLinkerTool': {
                                  'LinkTimeCodeGeneration': '1',      # useLinkTimeCodeGeneration /LTCG
                              },
                              'VCLibrarianTool': {
                                  'LinkTimeCodeGeneration': 'true',   # useLinkTimeCodeGeneration /LTCG
                              },
                          },
                      },
                  },
                  }],
             ],
         },
         ],

        # The following section is common to linux + derivatives and android
        ['OS == "linux"',
         {
             'cflags': [
                 '-g',
                 '-fno-exceptions',
                 '-fstrict-aliasing',

                 '-Wall',
                 '-Wextra',
                 '-Winit-self',
                 '-Wpointer-arith',
                 '-Wsign-compare',

                 '-Wno-unused-parameter',

                 '-m64',
             ],
             'cflags_cc': [
                 '-std=c++11',
                 '-frtti',
                 '-Wnon-virtual-dtor',
                 # GCC <4.6 is old-school strict about what is POD.
                 '-Wno-invalid-offsetof',
             ],
             'ldflags': [
                 '-m64',
             ],

             'configurations': {
                 'Coverage': {
                     'cflags': ['--coverage'],
                     'ldflags': ['--coverage'],
                 },
                 'Debug': {
                 },
                 'Release': {
                     'cflags': [
                         '-O3',
                     ],
                     'defines': ['NDEBUG'],
                 },
             },
         },
         ],

        ['OS == "mac"',
         {
             'configurations': {
                 'Coverage': {
                     'xcode_settings': {
                         'GCC_OPTIMIZATION_LEVEL': '0',
                         'GCC_GENERATE_TEST_COVERAGE_FILES': 'YES',
                         'GCC_INSTRUMENT_PROGRAM_FLOW_ARCS': 'YES',
                         'GCC_ENABLE_CPP_RTTI': 'YES',
                     },
                 },
                 'Debug': {
                     'xcode_settings': {'GCC_OPTIMIZATION_LEVEL': '0'},
                 },
                 'Release': {
                     'xcode_settings': {'GCC_OPTIMIZATION_LEVEL': '3', },
                     'defines': ['NDEBUG'],
                 },
             },
             'xcode_settings': {
                 'OTHER_CPLUSPLUSFLAGS': ['-std=c++0x', '-stdlib=libc++', ],
                 'ARCHS': ['x86_64'],
                 'MACOSX_DEPLOYMENT_TARGET': '10.8',

                 'CLANG_CXX_LANGUAGE_STANDARD': 'c++0x',
                 'GCC_ENABLE_SUPPLEMENTAL_SSE3_INSTRUCTIONS': 'YES',  # -mssse3
                 'GCC_SYMBOLS_PRIVATE_EXTERN': 'NO',   # -fvisibility=hidden
                 'GCC_INLINES_ARE_PRIVATE_EXTERN': 'NO',   # -fvisibility-inlines-hidden
                 'GCC_CW_ASM_SYNTAX': 'NO',   # remove -fasm-blocks
                 'GCC_ENABLE_PASCAL_STRINGS': 'NO',   # remove -mpascal-strings
                 'GCC_WARN_ABOUT_INVALID_OFFSETOF_MACRO': 'NO',   # -Wno-invalid-offsetof
                 'WARNING_CFLAGS': [
                     '-Wall',
                     '-Wextra',
                     '-Winit-self',
                     '-Wpointer-arith',
                     '-Wsign-compare',

                     '-Wno-unused-parameter',

                     '-Wno-unused-local-typedefs',
                     '-Wno-reorder',
                 ],
             },
         },
         ],
    ],  # end 'conditions'
    'xcode_settings': {
        'SYMROOT': '<(DEPTH)/build',
    },
}
