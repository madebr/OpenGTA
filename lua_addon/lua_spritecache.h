#ifndef LUA_OGTA_SPRITECACHE_H
#define LUA_OGTA_SPRITECACHE_H

#include "gl_spritecache.h"
#include "lua.hpp"

namespace OpenGTA {
  namespace Script {

    class SpriteCache {
      public:
        static int getScale2x(lua_State *L);
        static int setScale2x(lua_State *L);

        static const luaL_reg methods[];
    };
  }
}
#endif
