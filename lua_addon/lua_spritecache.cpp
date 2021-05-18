#include "lua_spritecache.h"

namespace OpenGTA {
  namespace Script {
using namespace OpenGL;
    int SpriteCache::getScale2x(lua_State *L) {
      bool b = SpriteCache::Instance().getScale2x();
      lua_pushboolean(L, b);
      return 1;
    }
    int SpriteCache::setScale2x(lua_State *L) {
      bool b = SpriteCache::Instance().getScale2x();
      bool v = lua_toboolean(L, 1);
      if (b != v)
        SpriteCache::Instance().setScale2x(v);
      return 0;
    }
#define method(name) {#name, SpriteCache::name}
    const luaL_reg SpriteCache::methods[] = {
      method(setScale2x),
      method(getScale2x),
      {NULL, NULL}
    };

  }
}
