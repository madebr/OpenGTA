#ifndef LUA_OGTA_CAMERA_H
#define LUA_OGTA_CAMERA_H

#include "gl_camera.h"
#include "lua.hpp"

namespace OpenGTA {
  namespace Script {

    class Camera {
      public:
        static int l_setSpeed(lua_State *L);
        static int l_setRotating(lua_State *L);
        static int l_setGravityOn(lua_State *L);
        static int l_interpolateToPosition(lua_State *L);
        static int l_getEye(lua_State *L);
        static int l_setEye(lua_State *L);
        static int l_getCenter(lua_State *L);
        static int l_setCenter(lua_State *L);
        static int l_getUp(lua_State *L);
        static int l_setUp(lua_State *L);

        /*static int mute(lua_State *L);
          static int unmute(lua_State *L);*/


        static const luaL_reg methods[];
    };
  }
}
#endif
