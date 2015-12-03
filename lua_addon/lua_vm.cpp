#include <string>
#include "lua_vm.h"
#include "lunar.h"
#include "lua_map.h"
#include "lua_cityview.h"
#include "lua_stackguard.h"
#include "lua_camera.h"
#include "lua_screen.h"
#include "lua_spritecache.h"
#include "m_exceptions.h"

using namespace Util;

namespace OpenGTA {
  namespace Script {
    LuaVM::LuaVM() : L(NULL) {
      L = lua_open();
      if (L == NULL)
        throw E_SCRIPTERROR("Failed to create Lua state!");
      
      luaopen_base(L);
      luaopen_math(L);
      _registered = false;
      lua_settop(L, 0);
      prepare();
    }

    LuaVM::~LuaVM() {
      if (L != NULL)
        lua_close(L);
      L = NULL;
    }

    void LuaVM::prepare() {
      LGUARD(L);
      if (!_registered) {
        Lunar<Block>::Register2(L);
        Lunar<LMap>::Register2(L);
      /*
        Lunar<CityView>::Register2(L);
        luaL_openlib(L, "camera",      Camera::methods, 0);
        luaL_openlib(L, "screen",      Screen::methods, 0);
        luaL_openlib(L, "spritecache", SpriteCache::methods, 0);
        */
      }
      _registered = true;
    }

    lua_State* LuaVM::getInternalState() {
      return(L);
    }

    void LuaVM::setMap(OpenGTA::Map & map) {
      LGUARD(L);
      LMap * mptr = static_cast<LMap*>(&map);
      lua_gettable(L, LUA_GLOBALSINDEX);
      int scv_ref = Lunar<LMap>::push(L, mptr, false);
      lua_pushliteral(L, "map");
      lua_pushvalue(L, scv_ref);
      lua_settable(L, LUA_GLOBALSINDEX);
    }

    void LuaVM::setCityView(OpenGTA::CityView & cv) {
      LGUARD(L);
      /*
      CityView *scv = static_cast<CityView*>(&cv);
      lua_gettable(L, LUA_GLOBALSINDEX);
      int scv_ref = Lunar<CityView>::push(L, scv, false);
      lua_pushliteral(L, "city_view");
      lua_pushvalue(L, scv_ref);
      lua_settable(L, LUA_GLOBALSINDEX);
      */
    }

    void LuaVM::runString(const char* _str) {
      LGUARD(L);
      if (!_str)
        return;
      if (luaL_loadbuffer(L, _str, strlen(_str), "cmd") ||
        lua_pcall(L, 0, 0, 0))
        throw E_SCRIPTERROR("Error running string: " + std::string(lua_tostring(L, -1)));
    }

    void LuaVM::runFile(const char* filename) {
      LGUARD(L);
      if (luaL_loadfile(L, filename) || lua_pcall(L, 0, 0, 0))
        throw E_SCRIPTERROR("Error running file: " + std::string(lua_tostring(L, -1)));
    }

    void LuaVM::callSimpleFunction(const char* func_name) {
      LGUARD(L);
      lua_getglobal(L, func_name);
      if (lua_type(L, -1) == LUA_TFUNCTION) {
        if (lua_pcall(L, 0, 0, 0) != 0)
          throw E_SCRIPTERROR(("Exception calling function: ") + std::string(lua_tostring(L, -1)));
      }
      else
        throw E_SCRIPTERROR("No such function: " + std::string(func_name));
    }

    int LuaVM::getGlobalInt(const char* key) {
      LGUARD(L);
      lua_getglobal(L, key);
      if (!lua_isnumber(L, -1))
        throw E_SCRIPTERROR("Expected int value for key: " + std::string(key));
      int v = int(lua_tonumber(L, -1));
      return v;
    }

    float LuaVM::getGlobalFloat(const char* key) {
      LGUARD(L);
      lua_getglobal(L, key);
      if (!lua_isnumber(L, -1))
        throw E_SCRIPTERROR("Expected float value for key: " + std::string(key));
      float v = float(lua_tonumber(L, -1));
      return v;
    }

    const char* LuaVM::getGlobalString(const char* key) {
      LGUARD(L);
      lua_getglobal(L, key);
      if (!lua_isstring(L, -1))
        throw E_SCRIPTERROR("Expected string value for key: " + std::string(key));
      const char* v = lua_tostring(L, -1);
      return v;
    }

    void LuaVM::setGlobalInt(const char* key, int v) {
      LGUARD(L);
      lua_pushnumber(L, v);
      lua_setglobal(L, key);
    }

    void LuaVM::setGlobalFloat(const char* key, float v) {
      LGUARD(L);
      lua_pushnumber(L, v);
      lua_setglobal(L, key);
    }

    void LuaVM::setGlobalString(const char* key, const char* v) {
      LGUARD(L);
      lua_pushstring(L, v);
      lua_setglobal(L, key);
    }

  }
}
