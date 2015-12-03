#include "lua_cityview.h"

namespace OpenGTA {
  namespace Script {
    
    int CityView::setCamPosition(lua_State *L) {
      float x = float(luaL_checknumber(L, 1));
      float y = float(luaL_checknumber(L, 2));
      float z = float(luaL_checknumber(L, 3));
      setPosition(x, y, z);
      return 0;
    }

    int CityView::getCamPosition(lua_State *L) {
      lua_pushnumber(L, camPos[0]);
      lua_pushnumber(L, camPos[1]);
      lua_pushnumber(L, camPos[2]);
      return 3;
    }

/*
    int CityView::setCamVector(lua_State *L) {
      float x = float(luaL_checknumber(L, 1));
      float y = float(luaL_checknumber(L, 2));
      float z = float(luaL_checknumber(L, 3));
      OpenGTA::CityView::setCamVector(x, y, z);
      return 0;
    }

    int CityView::getCamVector(lua_State *L) {
      lua_pushnumber(L, camVec[0]);
      lua_pushnumber(L, camVec[1]);
      lua_pushnumber(L, camVec[2]);
      return 3;
    }
*/

    int CityView::setTopDownView(lua_State *L) {
      setViewMode(lua_toboolean(L, 1));
      return 0;
    }

    int CityView::setDrawHeadingArrows(lua_State *L) {
      OpenGTA::CityView::setDrawHeadingArrows(lua_toboolean(L, 1));
      return 0;
    }

    int CityView::setVisibleRange(lua_State *L) {
      OpenGTA::CityView::setVisibleRange(luaL_checkinteger(L, 1));
      return 0;
    }

    int CityView::getVisibleRange(lua_State *L) {
      lua_pushnumber(L, visibleRange);
      return 1;
    }
    const char CityView::className[] = "CityView";
#define method(name) {#name, &CityView::name}
    Lunar<CityView>::RegType CityView::methods[] = {
      method(setCamPosition),
      method(getCamPosition),
//      method(setCamVector),
//      method(getCamVector),
      method(setTopDownView),
      method(setVisibleRange),
      method(getVisibleRange),
      method(setDrawHeadingArrows),
      {0, 0}
    };
  }
}
