#include "lua_screen.h"

namespace OpenGTA {
  namespace Script {
using namespace OpenGL;
    int Screen::getFullscreen(lua_State *L) {
      bool b = Screen::Instance().getFullscreen();
      lua_pushboolean(L, b);
      return 1;
    }
    int Screen::setFullscreen(lua_State *L) {
      bool b = Screen::Instance().getFullscreen();
      bool v = lua_toboolean(L, 1);
      if (b != v)
        Screen::Instance().toggleFullscreen();
      return 0;
    }
    int Screen::makeScreenShot(lua_State *L) {
      Screen::Instance().makeScreenshot(luaL_checkstring(L, 1));
      return 0;
    }
#define method(name) {#name, Screen::name}
    const luaL_reg Screen::methods[] = {
      method(setFullscreen),
      method(getFullscreen),
      method(makeScreenShot),
      {NULL, NULL}
    };

  }
}
