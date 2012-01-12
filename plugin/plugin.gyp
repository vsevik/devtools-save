# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'targets': [
    {
      'target_name': 'devtools-save',
      'type': 'loadable_module',
      'sources': [
        'devtools_save.cc',
        'logging.cc',
        'npp_entry.cc',
        'nputils.cc',
        'npclass_impl.cc',
      ],
      'include_dirs': [
        '<(DEPTH)'
      ],
      'conditions': [
        ['OS=="linux"', {
          'cflags': [
            '-fPIC'
          ]
        }]
      ]
    }
  ]
}
