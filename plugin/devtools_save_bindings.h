#ifndef _DEVTOOLS_SAVE_BINDINGS_H
#define _DEVTOOLS_SAVE_BINDINGS_H

#include "devtools_save.h"
#include "nputils.h"

class DevToolsSaveBindings :
    public NPObjectWithDispatcher<DevToolsSaveBindings>  {
 public:
  DevToolsSaveBindings() {}

 private:
  friend class NPObjectWithDispatcher<DevToolsSaveBindings>;
  static const NPClassDispatcher* dispatcher();

  bool Save(const NPVariant* args, uint32_t argCount,
            NPVariant* result);
  bool TestPath(const NPVariant* args, uint32_t argCount,
                NPVariant* result);

  DevToolsSave devtools_save_;
};

#endif // _DEVTOOLS_SAVE_BINDINGS_H
