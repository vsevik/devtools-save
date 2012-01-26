// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef NPUTILS_H
#define NPUTILS_H

#include "config.h"
#include <string>

#include "third_party/npapi/npfunctions.h"

struct NPUtils {
  static NPNetscapeFuncs* s_funcs;

  static bool Throw(NPObject* object, const char* message);
  static bool StringFromNPVariant(std::string* result, NPVariant variant);
};

class Identifier {
 public:
  explicit Identifier(NPIdentifier identifier);
  ~Identifier();

  bool operator==(const char* name) const;

  const char* AsUTF8() const {
    return name_;
  }   

 private:
  char* name_;

  Identifier();
  Identifier(const Identifier&);
};

template<typename T, size_t N> size_t ArraySize(T (&)[N]) {
  return N;
}

#endif // NPUTILITIES_H
