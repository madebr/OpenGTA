#ifndef LUA_OGTA_SCREEN_H
#define LUA_OGTA_SCREEN_H

#include "gl_screen.h"
#include "lua.hpp"

namespace OpenGTA {
  namespace Script {

    class Screen {
      public:
        static int getFullscreen(lua_State *L);
        static int setFullscreen(lua_State *L);
        static int makeScreenShot(lua_State *L);

        static const luaL_reg methods[];
    };
  }
}
#endif
