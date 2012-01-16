# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    '../build/common.gypi',
    '../build/plugin-version.gypi'
  ],
  'variables': {
    'conditions': [
      [ 'OS=="mac"', {
          'plugin_file': 'devtools-save.plugin',
        }, {
          'plugin_file': '<(SHARED_LIB_PREFIX)devtools-save<(SHARED_LIB_SUFFIX)',
        }
      ]
    ]
  },
  'targets': [
    {
      'target_name': 'devtools-save',
      'type': 'loadable_module',
      'mac_bundle': 1,
      'sources': [
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
      'include_dirs': [
        '<(DEPTH)'
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '-fPIC'
          ]
        }],
        ['OS=="mac"', {
          'product_extension': 'plugin'
        }],
        ['OS=="win"', {
          'sources': [ 'plugin.rc' ]
        }],
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
      'dependencies': [ 'devtools-save' ],
      'copies': [
        {
          'destination': '<(devtools-save_staging_area)/<(devtools_plugin_version)',
          'files': [ '<(PRODUCT_DIR)/<(plugin_file)' ]
        }
      ]
    }
  ]
}
