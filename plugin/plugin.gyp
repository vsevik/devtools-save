# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../build/common.gypi',
    '../build/version.gypi'
  ],
  'variables': {
    'plugin_sources': [
      'config.h',
      'devtools_save.cc',
      'devtools_save.h',
      'logging.cc',
      'logging.h',
      'npp_entry.cc',
      'nputils.cc',
      'nputils.h',
      'npclass_impl.cc',
      'npclass_impl.h',
    ],
    'conditions': [
      [ 'OS=="mac"', {
          'plugin_files': [ '<(PRODUCT_DIR)/devtools-save.plugin', ]
        }
      ],
      [ 'OS=="linux"', {
          'plugin_files': [
            '<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)devtools-save-x64'
                '<(SHARED_LIB_SUFFIX)',
            '<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)devtools-save-ia32'
                '<(SHARED_LIB_SUFFIX)',
          ]
        }
      ],
      [ 'OS=="win"', {
          'plugin_files': [
            '<(PRODUCT_DIR)/<(SHARED_LIB_PREFIX)devtools-save'
                '<(SHARED_LIB_SUFFIX)'
          ]
        }
      ],
    ]
  },
  'targets': [
    {
      'type': 'loadable_module',
      'target_name': 'devtools-save',
      'mac_bundle': 1,
      'sources': [
        '<@(plugin_sources)'
      ],
      'include_dirs': [
        '<(DEPTH)'
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '-fPIC',
          ],
          'product_name': 'devtools-save-x64'
        }],
        ['OS=="mac"', {
          'product_extension': 'plugin'
        }],
        ['OS=="win"', {
          'sources': [ 'plugin.rc' ]
        }]
      ],
      'msvs_settings': {
        'VCLinkerTool': {
	  'ModuleDefinitionFile': 'plugin.def',
	},
      },
      'xcode_settings': {
        'INFOPLIST_FILE': 'Info.plist'
      }
    },
    {
      'target_name': 'publish-plugin',
      'type': 'none',
      'dependencies': [
        'devtools-save'
      ],
      'copies': [
        {
          'files': [ '<@(plugin_files)' ],
          'destination': '<(devtools-save_staging_area)/'
              '<(devtools_plugin_version)/'
        }
      ],
      'conditions': [
        [ 'OS=="linux"', {
            'dependencies': [
              'devtools-save-ia32'
            ],
          }
        ]
      ]
    }
  ],
  'conditions': [
    [ 'OS=="linux"', {
        'targets': [
          {
            'target_name': 'devtools-save-ia32',
            'type': 'loadable_module',
            'sources': [
              '<@(plugin_sources)'
            ],
            'include_dirs': [
              '<(DEPTH)'
            ],
            'cflags': [
              '-fPIC',
              '-m32'
            ],
            'ldflags': [
              '-m32'
            ]
          }
        ]
      }
    ]
  ]
}
