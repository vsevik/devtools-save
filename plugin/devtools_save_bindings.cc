// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "devtools_save_bindings.h"

#include "logging.h"
#include "nputils.h"

bool DevToolsSaveBindings::Save(const NPVariant* args, uint32_t argCount,
                                NPVariant* result) {
  if (argCount != 2)
    return Throw("save(): invalid number of arguments, two expected");

  std::string filename, content;
  if (!NPUtils::StringFromNPVariant(&filename, args[0]))
    return Throw("save(): expected string as 'filename' argument");
  if (!NPUtils::StringFromNPVariant(&content, args[1]))
    return Throw("save(): expected string as 'content' argument");
  DevToolsSave::ResultCode rc = devtools_save_.Save(filename.c_str(),
                                                    content.c_str());
  INT32_TO_NPVARIANT(rc, *result);
  return true;
}

bool DevToolsSaveBindings::TestPath(const NPVariant* args, uint32_t argCount,
                                    NPVariant* result) {
  if (argCount != 1)
    return Throw("testPath(): invalid number of arguments, one expected");
  std::string path;
  if (!NPUtils::StringFromNPVariant(&path, args[0]))
    return Throw("testPath(): expected string as 'path' argument");
  DevToolsSave::ResultCode rc = devtools_save_.TestPath(path.c_str());
  if (result)
    INT32_TO_NPVARIANT(rc, *result);
  return true;
}

struct EnumPropertyValue {
  const char *name;
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

const NPClassDispatcher* DevToolsSaveBindings::dispatcher() {
  static NPClassDispatcher* dispatcher;
  if (dispatcher != NULL)
    return dispatcher;
  dispatcher = new NPClassDispatcher();
  dispatcher->AddMethod("save",
      static_cast<NPClassDispatcher::MethodCallback>(
          &DevToolsSaveBindings::Save));
  dispatcher->AddMethod("testPath",
      static_cast<NPClassDispatcher::MethodCallback>(
          &DevToolsSaveBindings::TestPath));
  for (unsigned int i = 0; i < ArraySize(properties); ++i) {
    NPVariant value;
    INT32_TO_NPVARIANT(properties[i].value, value);
    dispatcher->AddConstant(properties[i].name, value);
  }
  return dispatcher;
}
