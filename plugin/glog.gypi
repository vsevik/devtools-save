# Copyright (c) 2011 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

{
  'variables': {
    'glog_src_dir': '<(DEPTH)/third_party/glog',
    'glog_root_dir': '<(SHARED_INTERMEDIATE_DIR)/glog',
    'glog_build_dir': '<(glog_root_dir)/build',
    'glog_lib': '<(glog_root_dir)/lib/libglog.a',
    'glog_include_dir': '<(glog_root_dir)/include',
  },

  'targets': [
    {
      'target_name': 'glog',
      'type': 'none',
      'actions': [
        {
          'action_name': 'build_glog',
          'inputs': [
            '<!@(find <(glog_src_dir) -name .svn -prune -o -type f -print)',
          ],
          'outputs': [
            '<(glog_lib)'
          ],
          'action': [
            'sh',
            '-c',
            'rm -rf  &&'
            'mkdir -p <(glog_build_dir) &&'
            'cp <(glog_src_dir)/configure <(glog_build_dir) &&'
            'cd <(glog_build_dir) &&'
            './configure --disable-shared --prefix=<(glog_root_dir) '
                '--srcdir=`dirname <(RULE_INPUT_PATH)` --with-pic &&'
            'make install'
          ]
        }
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          '<(glog_include_dir)',
        ],
        'libraries': [
          '<(glog_lib)'
        ]
      }
    }
  ]
}
