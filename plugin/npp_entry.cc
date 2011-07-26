// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "npclass_impl.h"
#include "nputils.h"

#include "glog/logging.h"
#include "third_party/npapi/npapi.h"
#include "third_party/npapi/npfunctions.h"

#define PLUGIN_MIME_TYPE "application/x-devtools-save-extension"

static void MaybeInitializeLogging(void) {
  static bool initialized = false;

  if (initialized)
    return;
  initialized = true;
  google::InitGoogleLogging("devtools-save");
  google::LogToStderr();
}

static const char kPluginName[] = "DevTools Save plugin";
static const char kPluginDescription[] = 
    "Native component of Chrome devtools-save extension";

class NPP_Implementation {
 public:
  static void GetEntryPoints(NPPluginFuncs* nppfuncs) {
    nppfuncs->size = sizeof(NPPluginFuncs);
    nppfuncs->version = (NP_VERSION_MAJOR << 8) | NP_VERSION_MINOR;

    nppfuncs->newp = New;
    nppfuncs->destroy = Destroy;
    nppfuncs->getvalue = GetValue;
    nppfuncs->setwindow = SetWindow;
    nppfuncs->event = 0;
  }

  static NPError GetValue(NPP instance, NPPVariable variable, void *ret_value) {
    switch(variable) {
      case NPPVpluginNameString:
        *static_cast<const char**>(ret_value) = kPluginName;
        break;

      case NPPVpluginDescriptionString:
        *static_cast<const char**>(ret_value) = kPluginDescription;
        break;

      case NPPVpluginScriptableNPObject: {
          if (!instance || instance->pdata != s_plugin) {
            LOG(ERROR) << "invalid instance pointer!";
            return NPERR_INVALID_INSTANCE_ERROR;
          }

          NPObject* npobject = s_plugin->GetNPObject();
          if (!npobject) {
            npobject = NPUtils::s_funcs->createobject(instance,
                                                      NPClassImpl::GetClass());
            s_plugin->SetNPObject(npobject);
          }
          if (npobject)
            NPUtils::s_funcs->retainobject(npobject);
        
          *static_cast<NPObject**>(ret_value) = s_plugin->GetNPObject();
          break;
        }

      default:
        return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
  }

 private:
  class Plugin {
   public:
    explicit Plugin(NPP npp)
        : npp_(npp)
        , npobject_(0) {
    }
    void SetNPObject(NPObject* object) {
      npobject_ = object;
    }
    NPObject* GetNPObject() const {
      return npobject_;
    }

   private:
    NPP npp_;
    NPObject* npobject_;
  };

  static Plugin* s_plugin;

  static NPError New(NPMIMEType pluginType, NPP instance, uint16_t mode,
      int16_t, char*[], char*[], NPSavedData*) {

    if (strcmp(PLUGIN_MIME_TYPE, pluginType)) {
      LOG(ERROR) << "request to create object of unexpected MIME type";
      return NPERR_GENERIC_ERROR;
    }
    if (mode != NP_EMBED) {
      LOG(ERROR) << "unsupported embedding mode!";
      return NPERR_GENERIC_ERROR;
    }
    if (!instance) {
      LOG(ERROR) << "invalid instance pointer";
      return NPERR_INVALID_INSTANCE_ERROR;
    }

    if (s_plugin) {
      LOG(ERROR) << "only one instance is supported for security reasons!";
      return NPERR_INVALID_INSTANCE_ERROR;
    }
    NPUtils::s_funcs->setvalue(instance, NPPVpluginWindowBool, 0);
    s_plugin = new Plugin(instance);
    instance->pdata = s_plugin;
    DLOG(INFO) << "created plugin instance.";

    return NPERR_NO_ERROR;
  }

  static NPError Destroy(NPP instance, NPSavedData**) {
    if (!instance || instance->pdata != s_plugin) {
      LOG(ERROR) << "invalid instance pointer!";
      return NPERR_INVALID_INSTANCE_ERROR;
    }
    delete s_plugin;
    s_plugin = 0;
    instance->pdata = 0;
    DLOG(INFO) << "destroyed plugin instance.";
    return NPERR_NO_ERROR;
  }

  static NPError SetWindow(NPP, NPWindow*) {
    return NPERR_NO_ERROR;
  }
};

NPP_Implementation::Plugin* NPP_Implementation::s_plugin = 0;

extern "C" {

NPError OSCALL NP_GetEntryPoints(NPPluginFuncs* nppfuncs) {
  NPP_Implementation::GetEntryPoints(nppfuncs);

  return NPERR_NO_ERROR;
}

NPError OSCALL NP_Initialize(NPNetscapeFuncs *aNPNFuncs
#if !defined(WIN32)
                             , NPPluginFuncs *aNPPFuncs
#endif
     ) {
  MaybeInitializeLogging();
  
  if (NPUtils::s_funcs) {
    LOG(ERROR) << "attempt to initialize second instance!";
    return NPERR_GENERIC_ERROR;
  }

  if (((aNPNFuncs->version >> 8) & 0xFF) > NP_VERSION_MAJOR) {
    LOG(ERROR) << "incompatible NPAPI version!";
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }

  if (!aNPNFuncs) {
    LOG(ERROR) << "invalid functions table passed!";
    return NPERR_INVALID_FUNCTABLE_ERROR;
  }
  NPUtils::s_funcs = aNPNFuncs;
#if !defined(_WIN32)
  NP_GetEntryPoints(aNPPFuncs);
#endif
  DLOG(INFO) << "initialized plugin.";
  return NPERR_NO_ERROR;
}

NPError OSCALL NP_Shutdown() {
  DLOG(INFO) << "shutting plugin down.";
  NPUtils::s_funcs = 0;
  google::ShutdownGoogleLogging();
  return NPERR_NO_ERROR;
}

const char* OSCALL NP_GetMIMEDescription(void) {
  MaybeInitializeLogging();

  return PLUGIN_MIME_TYPE "::DevTools Save Extension Plugin";
}

NPError OSCALL NP_GetValue(void* instance,
    NPPVariable variable, void *ret_value) {
  NPP_Implementation::GetValue(static_cast<NPP>(instance), variable, ret_value);
}

} // extern "C"
