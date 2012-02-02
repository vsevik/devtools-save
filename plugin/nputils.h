// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _NPUTILS_H
#define _NPUTILS_H

#include "config.h"
#include <map>
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

template<typename DerivedClass>
class NPObjectWithDispatcher: public NPObject {
 public:
  static NPClass* GetClass() {
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

  static NPObject* Allocate(NPP npp, NPClass* aClass) {
    return new DerivedClass();
  }

  static void Deallocate(NPObject* object) {
    delete static_cast<DerivedClass*>(object);
  }

  static bool HasMethod(NPObject* object, NPIdentifier identifier) {
    return DerivedClass::dispatcher()->HasMethod(identifier);
  }

  static bool HasProperty(NPObject* object, NPIdentifier identifier) {
    return DerivedClass::dispatcher()->HasProperty(identifier);
  }

  static bool Invoke(NPObject* object, NPIdentifier identifier,
                     const NPVariant* args, uint32_t argCount,
                     NPVariant* result) {
    return DerivedClass::dispatcher()->Invoke(object, identifier, args,
                                              argCount, result);
  }

  static bool InvokeDefault(NPObject*, const NPVariant*, uint32_t, NPVariant*) {
    return false;
  }

  static bool GetProperty(NPObject* object, NPIdentifier identifier,
                          NPVariant* result) {
    return DerivedClass::dispatcher()->GetProperty(object, identifier, result);
  }

  static bool SetProperty(NPObject* object, NPIdentifier identifier,
                          const NPVariant* value) {
    return DerivedClass::dispatcher()->SetProperty(object, identifier, value);
  }

 protected:
  NPObjectWithDispatcher() {}
  bool Throw(const char* message) {
    return NPUtils::Throw(this, message);
  }

 private:
  NPObjectWithDispatcher(const NPObjectWithDispatcher&);
  NPObjectWithDispatcher& operator=(const NPObjectWithDispatcher&);
};

class NPClassDispatcher {
 public:
  typedef bool (NPObject::*MethodCallback)(const NPVariant* args, 
      uint32_t argCount, NPVariant* result);
  typedef bool (NPObject::*GetterCallback)(NPVariant* result);
  typedef bool (NPObject::*SetterCallback)(const NPVariant* value);

  NPClassDispatcher() {}
  ~NPClassDispatcher();

  void AddMethod(const char* name, MethodCallback);
  void AddProperty(const char* name, GetterCallback, SetterCallback);
  void AddConstant(const char* name, NPVariant value);

  bool HasMethod(NPIdentifier) const;
  bool HasProperty(NPIdentifier) const;

  bool Invoke(NPObject* object, NPIdentifier,
              const NPVariant* args, uint32_t argCount,
              NPVariant* result) const;
  bool GetProperty(NPObject* object, NPIdentifier, NPVariant* result) const;
  bool SetProperty(NPObject* object, NPIdentifier,
                   const NPVariant* value) const;

 private:
  enum PropertyType {
    kConstant,
    kMethod,
    kProperty
  };

  struct GetterAndSetter {
    GetterCallback getter;
    SetterCallback setter;
  };

  struct PropertyDescriptor {
    PropertyType type;
    union {
      MethodCallback method;
      GetterAndSetter getter_and_setter;
      NPVariant value;
    };
  };

  typedef std::map<std::string, PropertyDescriptor> PropertyMap;

  const PropertyDescriptor*
      GetPropertyDescriptor(NPIdentifier identifier) const;
  void SetPropertyDescriptor(const char*, const PropertyDescriptor&);

  PropertyMap properties_;

  NPClassDispatcher(const NPClassDispatcher&);
  NPClassDispatcher& operator=(const NPClassDispatcher&);
};

#endif // _NPUTILS_H
