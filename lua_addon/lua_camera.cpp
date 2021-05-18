#include "lua_camera.h"
#define method(name) {#name, Camera::l_##name}

namespace OpenGTA {
  namespace Script {

using namespace OpenGL;

    int Camera::l_setSpeed(lua_State *L) {
      float tmp = float(luaL_checknumber(L, 1));
      Camera::Instance().setSpeed(tmp);
      return 0;
    }
    int Camera::l_setRotating(lua_State *L) {
      bool b = lua_toboolean(L, 1);
      Camera::Instance().setRotating(b);
      return 0;
    }
    int Camera::l_getEye(lua_State *L) {
      Vector3D & e = Camera::Instance().getEye();
      lua_pushnumber(L, e.x);
      lua_pushnumber(L, e.y);
      lua_pushnumber(L, e.z);
      return 3;
    }
    int Camera::l_setEye(lua_State *L) {
      Vector3D & e = Camera::Instance().getEye();
      e.x = luaL_checknumber(L, 1);
      e.y = luaL_checknumber(L, 2);
      e.z = luaL_checknumber(L, 3);
      return 0;
    }
    int Camera::l_getCenter(lua_State *L) {
      Vector3D & e = Camera::Instance().getCenter();
      lua_pushnumber(L, e.x);
      lua_pushnumber(L, e.y);
      lua_pushnumber(L, e.z);
      return 3;
    }
    int Camera::l_setCenter(lua_State *L) {
      Vector3D & e = Camera::Instance().getCenter();
      e.x = luaL_checknumber(L, 1);
      e.y = luaL_checknumber(L, 2);
      e.z = luaL_checknumber(L, 3);
      return 0;
    }
    int Camera::l_getUp(lua_State *L) {
      Vector3D & e = Camera::Instance().getUp();
      lua_pushnumber(L, e.x);
      lua_pushnumber(L, e.y);
      lua_pushnumber(L, e.z);
      return 0;
    }
    int Camera::l_setUp(lua_State *L) {
      Vector3D & e = Camera::Instance().getUp();
      e.x = luaL_checknumber(L, 1);
      e.y = luaL_checknumber(L, 2);
      e.z = luaL_checknumber(L, 3);
      return 0;
    }
    int Camera::l_setGravityOn(lua_State *L) {
      bool v = lua_toboolean(L, 1);
      Camera::Instance().setCamGravity(v);
      return 0;
    }
    int Camera::l_interpolateToPosition(lua_State *L) {
      float x, y, z;
      x = float(luaL_checknumber(L, 1));
      y = float(luaL_checknumber(L, 2));
      z = float(luaL_checknumber(L, 3));
      Uint32 msecInterval = Uint32(luaL_checkinteger(L, 4));
      Camera::Instance().interpolate(Vector3D(x, y, z), 1, msecInterval);
      return 0;
    }


    const luaL_reg Camera::methods[] = {
      method(setSpeed),
      method(setRotating),
      method(setEye),
      method(getEye),
      method(setCenter),
      method(getCenter),
      method(getUp),
      method(setUp),
      method(setGravityOn),
      method(interpolateToPosition),
      {NULL, NULL}
    };
  }
}
