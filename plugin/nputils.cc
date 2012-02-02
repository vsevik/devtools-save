// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "nputils.h"

#include <string.h>
#include "logging.h"

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

bool NPClassDispatcher::HasMethod(NPIdentifier identifier) const {
  const PropertyDescriptor* property = GetPropertyDescriptor(identifier);
  return property != NULL && property->type == kMethod;
}

bool NPClassDispatcher::HasProperty(NPIdentifier identifier) const {
  const PropertyDescriptor* property = GetPropertyDescriptor(identifier);
  return property != NULL &&
      (property->type == kProperty || property->type == kConstant);
}

bool NPClassDispatcher::Invoke(NPObject* object, NPIdentifier identifier,
                               const NPVariant* args, uint32_t argCount,
                               NPVariant* result) const {
  const PropertyDescriptor* property = GetPropertyDescriptor(identifier);
  if (property == NULL)
    return false;
  if (property->type != kMethod)
    return false;
  return (object->*property->method)(args, argCount, result);
}

bool NPClassDispatcher::GetProperty(NPObject* object, NPIdentifier identifier,
                                    NPVariant *result) const {
  if (result == NULL)
    return false;
  const PropertyDescriptor* property = GetPropertyDescriptor(identifier);
  if (property == NULL)
    return false;
  switch (property->type) {
  case kProperty:
    return (object->*property->getter_and_setter.getter)(result);
  case kConstant:
    *result = property->value;
    return true;
  default:
    return false;
  }
}

bool NPClassDispatcher::SetProperty(NPObject* object,  NPIdentifier identifier,
                                    const NPVariant *value) const {
  const PropertyDescriptor* property = GetPropertyDescriptor(identifier);
  if (property == NULL)
    return false;
  if (property->type != kProperty)
    return false;
  if (property->getter_and_setter.setter == NULL)
    return false;
  return (object->*property->getter_and_setter.setter)(value);
}

const NPClassDispatcher::PropertyDescriptor*
    NPClassDispatcher::GetPropertyDescriptor(NPIdentifier identifier) const {
  std::string name(Identifier(identifier).AsUTF8());
  PropertyMap::const_iterator it = properties_.find(name);
  if (it == properties_.end()) {
    LOG(ERROR) << "Missing property: " << name;
    return NULL;
  }
  return &(it->second);
}

void NPClassDispatcher::SetPropertyDescriptor(const char* name,
    const PropertyDescriptor& descriptor) {
  PropertyMap::iterator it = properties_.find(name);
  if (it != properties_.end()) {
    if (it->second.type == kConstant)
      NPUtils::s_funcs->releasevariantvalue(&(it->second.value));
    it->second = descriptor;
  } else {
    properties_[name] = descriptor;
  }
}

void NPClassDispatcher::AddMethod(const char* name, MethodCallback method) {
  PropertyDescriptor descriptor;
  descriptor.type = kMethod;
  descriptor.method = method;
  SetPropertyDescriptor(name, descriptor);
}

void NPClassDispatcher::AddProperty(const char* name,
                                    GetterCallback getter,
                                    SetterCallback setter) {
  PropertyDescriptor descriptor;
  descriptor.type = kProperty;
  descriptor.getter_and_setter.getter = getter;
  descriptor.getter_and_setter.setter = setter;
  SetPropertyDescriptor(name, descriptor);
}

void NPClassDispatcher::AddConstant(const char* name, NPVariant value) {
  PropertyDescriptor descriptor;
  descriptor.type = kConstant;
  descriptor.value = value;
  SetPropertyDescriptor(name, descriptor);
}

NPClassDispatcher::~NPClassDispatcher() {
  for (PropertyMap::iterator it = properties_.begin();
       it != properties_.end(); ++it) {
    if (it->second.type == kConstant)
      NPUtils::s_funcs->releasevariantvalue(&(it->second.value));
  }
  properties_.clear();
}
