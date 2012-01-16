// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NPCLASS_IMPL_H_
#define NPCLASS_IMPL_H_

#include "config.h"
#include "third_party/npapi/npfunctions.h"

class NPClassImpl {
 public:
  static NPClass* GetClass();
  static void SetNPNFunctions(NPNetscapeFuncs* functions);
};

#endif // NPCLASS_IMPL_H_
