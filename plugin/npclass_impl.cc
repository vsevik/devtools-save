// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "npclass_impl.h"

#include <string.h>

#include "devtools_save.h"
#include "nputils.h"
#include "glog/logging.h"
#include "third_party/npapi/npfunctions.h"

namespace {

NPObject *Allocate(NPP npp, NPClass* aClass) {
  return new DevToolsSave();
}

void Deallocate(NPObject* npobj) {
  delete npobj;
}

bool HasMethod(NPObject* npobj, NPIdentifier name) {
  Identifier identifier(name);
  if (identifier == "save")
    return true;
  return false;
}

bool Invoke(NPObject* npobj, NPIdentifier name,
            const NPVariant* args, uint32_t argCount,
            NPVariant* result) {
  Identifier identifier(name);
  DevToolsSave* instance = static_cast<DevToolsSave*>(npobj);

  if (result)
    NULL_TO_NPVARIANT(*result);

  DLOG(INFO) << "Invoke(" << identifier.AsUTF8() << "," << argCount << ")";
  if (identifier == "save") {
    if (argCount != 2) {
      return NPUtils::Throw(instance,
          "save(): invalid number of arguments, two expected");
    }
    std::string filename, content;
    if (!NPUtils::StringFromNPVariant(&filename, args[0])) {
      return NPUtils::Throw(instance,
                            "save(): expected string as 'filename' argument");
    }
    if (!NPUtils::StringFromNPVariant(&content, args[1])) {
      return NPUtils::Throw(instance,
                            "save(): expected string as 'content' argument");
    }
    instance->Save(filename.c_str(), content.c_str());
    return true;    
  }
  return false;
}

bool InvokeDefault(NPObject*,
                   const NPVariant*,
                   uint32_t,
                   NPVariant* result) {
  if (result)
    NULL_TO_NPVARIANT(*result);
  return true;
}

}

NPClass* NPClassImpl::GetClass() {
  static NPClass s_npclass = {
    NP_CLASS_STRUCT_VERSION,
    Allocate,
    Deallocate,
    0,  // Invalidate is not implemented
    HasMethod,
    Invoke,
    InvokeDefault,
    0,  // HasProperty is not implemented
    0,  // GetProperty is not implemented
    0,  // SetProperty is not implemented
  };

  return &s_npclass;
}
