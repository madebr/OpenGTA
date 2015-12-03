#ifndef LUA_MAP_H
#define LUA_MAP_H

#include "lunar.h"
#include "opengta.h"

namespace OpenGTA {
  namespace Script {
    class Block : public OpenGTA::Map::BlockInfo {
      public:
        int l_getBlockType(lua_State *L);
        int l_getUpOk(lua_State *L);
        int l_getDownOk(lua_State *L);
        int l_getLeftOk(lua_State *L);
        int l_getRightOk(lua_State *L);
        int l_getIsFlat(lua_State *L);
        int l_getSlopeType(lua_State *L);
        int l_getRotation(lua_State *L);
        int l_getRailway(lua_State *L);
        int l_getRailStation(lua_State *L);
        int l_getRailStationTrain(lua_State *L);
        int l_getTextureId(lua_State *L);

        int l_setBlockType(lua_State *L);
        int l_setUpOk(lua_State *L);
        int l_setDownOk(lua_State *L);
        int l_setLeftOk(lua_State *L);
        int l_setRightOk(lua_State *L);
        int l_setIsFlat(lua_State *L);
        int l_setSlopeType(lua_State *L);
        int l_setRotation(lua_State *L);
        //...
        int l_setTextureId(lua_State *L);


        static const char className[];
        static Lunar<Block>::RegType methods[];
    };
    class LMap : public OpenGTA::Map {
      public:
       LMap() : OpenGTA::Map("") {}
       int l_getNumBlocksAt(lua_State *L);
       int l_getBlockAt(lua_State *L);

       static const char className[];
       static Lunar<LMap>::RegType methods[];
    };
  }
}
#endif
