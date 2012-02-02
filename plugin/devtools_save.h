// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVTOOLS_SAVE_H_
#define DEVTOOLS_SAVE_H_

#include "config.h"
#include "third_party/npapi/npfunctions.h"

class DevToolsSave {
 public:
  enum ResultCode {
    ERR_OK,
    ERR_NOT_FOUND,
    ERR_NO_ACCESS,
    ERR_EXECUTABLE,
    ERR_BACKREFERENCE,
    ERR_RELATIVE_PATH,
    ERR_MISSING_ALLOW_DEVTOOLS,
    ERR_WRITE_FAILED,
    ERR_INTERNAL
  };
  ResultCode Save(const char* filename, const char* content);
  ResultCode TestPath(const char* path);
};

#endif // DEVTOOLS_SAVE_H_
