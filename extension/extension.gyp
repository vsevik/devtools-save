# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'includes': [
    '../build/common.gypi',
    '../build/plugin-version.gypi'
  ],
  'variables': {
    'extension_crx_file': '<(PRODUCT_DIR)/devtools-save.crx',
    'extension_files': [
      'background.html',
      'background.js',
      'devtools.html',
      'file_mapping.js',
      'grid.css',
      'grid.js',
      'manifest.json',
      'mapping_editor.js',
      'config-notification.html',
      'options.html',
    ],
    'extension_images': [
      'img/error.png',
      'img/information.png',
      'img/remove.png',
      'img/saved-ok.png'
    ],
    'plugin_file_linux': 'libdevtools-save.so',
    'plugin_file_mac': 'devtools-save.plugin',
    'plugin_file_win': 'devtools-save.dll',
    'conditions': [
      [
        'OS=="win"', {
          'crxmake': '<(DEPTH)/build/crxmake_win.sh',
        }, {
          'crxmake': '<(DEPTH)/build/crxmake.sh',
        }
      ]
    ]
  },
  'targets': [
    {
      'target_name': 'devtools-save',
      'type': 'none',
      'dependencies': [
        'copy-extension-files'
      ],
      'actions': [
        {
          'action_name': 'pack-extension',
          'inputs': [ 
            '<@(extension_files)',
          ],
          'outputs': [ '<(extension_crx_file)' ],
          'action': [
            'bash',
            '-e',  
            '<(crxmake)',
            '<(SHARED_INTERMEDIATE_DIR)/devtools-save',
            '<(devtools-save_pem_file)',
            '<@(_outputs)'
          ]
        }
      ]
    },
    {
      'target_name': 'copy-extension-files',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/plugin/plugin.gyp:publish-plugin',
      ],
      'copies': [
        {
          'destination': '<(SHARED_INTERMEDIATE_DIR)/devtools-save',
          'files': [
            '<@(extension_files)',
          ]
        },
        {
          'destination': '<(SHARED_INTERMEDIATE_DIR)/devtools-save/img',
          'files': [ '<@(extension_images)' ]
        }
      ],
      'actions': [
        {
          'action_name': 'pull-in-plugin-binaries',
          'variables': {
            'devtools_plugin_files': [
              '<(devtools-save_staging_area)/<(devtools_plugin_version)/<(plugin_file_linux)',
              '<(devtools-save_staging_area)/<(devtools_plugin_version)/<(plugin_file_win)',
              '<(devtools-save_staging_area)/<(devtools_plugin_version)/<(plugin_file_mac)',
            ]
          },
          'inputs': [
          ],
          'outputs': [
            '<(SHARED_INTERMEDIATE_DIR)/devtools-save/<(plugin_file_linux)',
            '<(SHARED_INTERMEDIATE_DIR)/devtools-save/<(plugin_file_win)',
            '<(SHARED_INTERMEDIATE_DIR)/devtools-save/<(plugin_file_mac)',
          ],
          'action': [
            'cp',
            '-R',
            '<@(devtools_plugin_files)',
            '<(SHARED_INTERMEDIATE_DIR)/devtools-save'
          ]
        }
      ]
    }
  ]
}
