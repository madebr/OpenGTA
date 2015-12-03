#ifndef OPENGTA_SCRIPT_VM_H
#define OPENGTA_SCRIPT_VM_H

#include "lua.hpp"
#include "Singleton.h"
#include "gl_cityview.h"

namespace OpenGTA {
  namespace Script {
    class LuaVM {
      public:
        LuaVM();
        ~LuaVM();
        void runString(const char*);
        void runFile(const char*);
        void callSimpleFunction(const char*);
        void prepare();
        void setCityView(OpenGTA::CityView &);
        int getGlobalInt(const char*);
        float getGlobalFloat(const char*);
        const char* getGlobalString(const char*);
        void setGlobalInt(const char*, int);
        void setGlobalFloat(const char*, float);
        void setGlobalString(const char*, const char*);
      protected:
        lua_State *L;
      private:
        bool _registered;
    };
    typedef Loki::SingletonHolder<LuaVM, Loki::CreateUsingNew, Loki::DefaultLifetime,
      Loki::SingleThreaded> LuaVMHolder;
  }
}
#endif
