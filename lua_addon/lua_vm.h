#ifndef OPENGTA_SCRIPT_VM_H
#define OPENGTA_SCRIPT_VM_H

#include "lua.hpp"
#include "gl_cityview.h"

namespace OpenGTA {
  namespace Script {
    class LuaVM {
      private:
        LuaVM();
        ~LuaVM();
      public:
        LuaVM(const LuaVM& copy) = delete;
        LuaVM& operator=(const LuaVM& copy) = delete;

        static LuaVM& Instance()
        {
          static LuaVM instance;
          return instance;
        }

        void  runString(const char*);
        void  runFile(const char*);
        void  callSimpleFunction(const char*);
        void  setCityView(OpenGTA::CityView &);
        void  setMap(OpenGTA::Map &);
        int   getGlobalInt(const char*);
        float getGlobalFloat(const char*);
        const char* getGlobalString(const char*);
        bool  getGlobalBool(const char*);
        void  setGlobalInt(const char*, int);
        void  setGlobalFloat(const char*, float);
        void  setGlobalString(const char*, const char*);
        void  setGlobalBool(const char*, bool);

        void  setInt(const char*, int);
        int   getInt(const char*);
        void  setFloat(const char*, float);
        float getFloat(const char*);
        void  setString(const char*, const char*);
        const char*  getString(const char*);
        void  setBool(const char*, bool);
        bool  getBool(const char*);

        lua_State *getInternalState();
      protected:
        lua_State *L;
      private:
        bool _registered;
        void prepare();
    };
  }
}
#endif
