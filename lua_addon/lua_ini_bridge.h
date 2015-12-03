#ifndef LUA_INI_BRIDGE_H
#define LUA_INI_BRIDGE_H

#include <string>
#include "read_ini.h"
#include "lua.hpp"

namespace OpenGTA {
  namespace Script {
    class IniScriptBridge : public ScriptParser {
      public:
        IniScriptBridge(const std::string &file);
        ~IniScriptBridge();
        lua_State *L;
        void loadLevel(PHYSFS_uint32 level);
      protected:
        void reset();
        void acceptDefinition(const char*);
        void acceptCommand(const char*);

    };
  }
}

#endif
