// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "npclass_impl.h"

#include <string.h>

#include "devtools_save.h"
#include "logging.h"
#include "nputils.h"
#include "third_party/npapi/npfunctions.h"

namespace {

NPObject *Allocate(NPP npp, NPClass* aClass) {
  return new DevToolsSave();
}

void Deallocate(NPObject* npobj) {
  DevToolsSave* instance = static_cast<DevToolsSave*>(npobj);
  delete instance;
}

bool HasMethod(NPObject* npobj, NPIdentifier name) {
  Identifier identifier(name);
  if (identifier == "save" || identifier == "testPath")
    return true;
  return false;
}

static bool Save(DevToolsSave* instance,
                 const NPVariant* args, uint32_t argCount,
                 NPVariant* result) {
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
  DevToolsSave::ResultCode rc = instance->Save(filename.c_str(),
                                               content.c_str());
  INT32_TO_NPVARIANT(rc, *result);
  return true;
}

static bool TestPath(DevToolsSave* instance,
                     const NPVariant* args, uint32_t argCount,
                     NPVariant* result) {
  if (argCount != 1) {
    return NPUtils::Throw(instance,
        "testPath(): invalid number of arguments, one expected");
  }
  std::string path;
  if (!NPUtils::StringFromNPVariant(&path, args[0])) {
    return NPUtils::Throw(instance,
                          "testPath(): expected string as 'path' argument");
  }
  DevToolsSave::ResultCode rc = instance->TestPath(path.c_str());
  if (result)
    INT32_TO_NPVARIANT(rc, *result);
  return true;
}

bool Invoke(NPObject* npobj, NPIdentifier name,
            const NPVariant* args, uint32_t argCount,
            NPVariant* result) {
  Identifier identifier(name);
  DevToolsSave* instance = static_cast<DevToolsSave*>(npobj);

  if (result)
    NULL_TO_NPVARIANT(*result);

  DLOG(INFO) << "Invoke(" << identifier.AsUTF8() << "," << argCount << ")";
  if (identifier == "save")
    return Save(instance, args, argCount, result);
  if (identifier == "testPath")
    return TestPath(instance, args, argCount, result);
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

struct EnumPropertyValue {
  const char* name;
  int value;
};

#define PROPERTY_MAP_ENTRY(name) \
    { #name, static_cast<int>(DevToolsSave::name) }

static EnumPropertyValue properties[] = {
  PROPERTY_MAP_ENTRY(ERR_OK),
  PROPERTY_MAP_ENTRY(ERR_RELATIVE_PATH),
  PROPERTY_MAP_ENTRY(ERR_BACKREFERENCE),
  PROPERTY_MAP_ENTRY(ERR_NOT_FOUND),
  PROPERTY_MAP_ENTRY(ERR_NO_ACCESS),
  PROPERTY_MAP_ENTRY(ERR_EXECUTABLE),
  PROPERTY_MAP_ENTRY(ERR_MISSING_ALLOW_DEVTOOLS),
  PROPERTY_MAP_ENTRY(ERR_WRITE_FAILED),
  PROPERTY_MAP_ENTRY(ERR_INTERNAL)
};

bool HasProperty(NPObject*, NPIdentifier name) {
  Identifier identifier(name);
  for (int i = ArraySize(properties) - 1; i >= 0; --i) {
    if (identifier == properties[i].name)
      return true;
  }
  return false;
}

bool GetProperty(NPObject*, NPIdentifier name,
                 NPVariant* result) {
  if (!result)
    return false;
  Identifier identifier(name);
  for (int i = ArraySize(properties) - 1; i >= 0; --i) {
    if (identifier == properties[i].name) {
      INT32_TO_NPVARIANT(properties[i].value, *result);
      return true;
    }
  }
  return false;
}

bool SetProperty(NPObject*, NPIdentifier, const NPVariant* result) {
  return false;
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
    HasProperty,
    GetProperty,
    SetProperty
  };

  return &s_npclass;
}
