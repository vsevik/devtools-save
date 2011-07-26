# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'includes': [
    'glog.gypi'
  ],
  'targets': [
    {
      'target_name': 'devtools-save',
      'type': 'loadable_module',
      'sources': [
        'npp_entry.cc',
        'nputils.cc',
        'npclass_impl.cc',
        'devtools_save.cc'
      ],
      'include_dirs': [
        '<(DEPTH)'
      ],
      'dependencies': [
        'glog'
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
