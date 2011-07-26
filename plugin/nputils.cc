// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nputils.h"

#include <string.h>
#include "glog/logging.h"

NPNetscapeFuncs* NPUtils::s_funcs = 0;

bool NPUtils::Throw(NPObject* object, const char* message) {
  LOG(ERROR) << "throwing exception: " << message;
  s_funcs->setexception(object, message);

  return true;
}

bool NPUtils::StringFromNPVariant(std::string* result, NPVariant variant) {
  if (!NPVARIANT_IS_STRING(variant))
    return false;
  const NPString& npstring = NPVARIANT_TO_STRING(variant);
  result->assign(npstring.UTF8Characters, npstring.UTF8Length);
  return true;
}

Identifier::Identifier(NPIdentifier identifier)
    : name_(NPUtils::s_funcs->utf8fromidentifier(identifier)) {
}

Identifier::~Identifier() {
  NPUtils::s_funcs->memfree(name_);
}

bool Identifier::operator==(const char* name) const {
  return !strcmp(name_, name);
}
