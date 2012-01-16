// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DEVTOOLS_SAVE_H_
#define DEVTOOLS_SAVE_H_

#include "config.h"
#include "third_party/npapi/npfunctions.h"

class DevToolsSave : public NPObject {
 public:
  void Save(const char* filename, const char* content);
};

#endif // DEVTOOLS_SAVE_H_
