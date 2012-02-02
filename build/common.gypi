# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
{
  'xcode_settings': {
    'SYMROOT': '<(DEPTH)/xcodebuild'
  },
  'conditions': [
    ['OS=="win"', {
        'target_defaults': {
          'msvs_cygwin_dirs': ['<(DEPTH)/third_party/cygwin']
        }
      }
    ],
    ['OS!="win"', {
        'target_defaults': {
          'cflags': ['-Wall', '-Werror']
        }
      }
    ]
  ]
}
