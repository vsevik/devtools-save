# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
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
      '<(PRODUCT_DIR)/libdevtools-save.so'
    ],
    'extension_images': [
      'img/error.png',
      'img/information.png',
      'img/remove.png',
      'img/saved-ok.png'
    ]
  },
  'targets': [
    {
      'target_name': 'devtools-save',
      'type': 'none',
      'inputs': [ '<@(extension_files)' ],
      'outputs': '<(INTERMEDIATE_DIR)/devtools-save.crx',
      'dependencies': [
        'copy-extension-files'
      ],
      'actions': [
        {
          'action_name': 'pack-extension',
          'inputs': [ '<@(extension_files)' ],
          'outputs': [ '<(extension_crx_file)' ],
          'action': [
            '<(DEPTH)/build/crxmake.sh',
            '<(INTERMEDIATE_DIR)/devtools-save',
            '<(DEPTH)/devtools-save.pem',
            '<@(_outputs)'
          ]
        }
      ]
    },
    {
      'target_name': 'copy-extension-files',
      'type': 'none',
      'dependencies': [
        '<(DEPTH)/plugin/plugin.gyp:devtools-save',
      ],
      'copies': [
        {
          'destination': '<(INTERMEDIATE_DIR)/devtools-save',
          'files': [ '<@(extension_files)' ]
        },
        {
          'destination': '<(INTERMEDIATE_DIR)/devtools-save/img',
          'files': [ '<@(extension_images)' ]
        }
      ]
    }
  ]
}
