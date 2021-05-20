#include <cassert>
#include "lua_map.h"
#include "m_exceptions.h"

namespace OpenGTA {
  namespace Script {
    int Block::l_getBlockType(lua_State *L) {
      lua_pushinteger(L, blockType());
      return 1;
    }
    int Block::l_setBlockType(lua_State *L) {
      uint8_t v = luaL_checkinteger(L, 1);
      setBlockType(v);
      return 0;
    }
    int Block::l_getSlopeType(lua_State *L) {
      lua_pushinteger(L, slopeType());
      return 1;
    }
    int Block::l_setSlopeType(lua_State *L) {
      uint8_t v = luaL_checkinteger(L, 1);
      setSlopeType(v);
      return 0;
    }
    int Block::l_getRotation(lua_State *L) {
      lua_pushinteger(L, rotation());
      return 1;
    }
    int Block::l_setRotation(lua_State *L) {
      uint8_t v = luaL_checkinteger(L, 1);
      setRotation(v);
      return 0;
    }
    int Block::l_getUpOk(lua_State *L) {
      lua_pushboolean(L, upOk());
      return 1;
    }
    int Block::l_getDownOk(lua_State *L) {
      lua_pushboolean(L, downOk());
      return 1;
    }
    int Block::l_getLeftOk(lua_State *L) {
      lua_pushboolean(L, leftOk());
      return 1;
    }
    int Block::l_getRightOk(lua_State *L) {
      lua_pushboolean(L, rightOk());
      return 1;
    }
    int Block::l_getIsFlat(lua_State *L) {
      lua_pushboolean(L, isFlat());
      return 1;
    }
    int Block::l_getRailway(lua_State *L) {
      lua_pushboolean(L, railway());
      return 1;
    }
    int Block::l_getRailStation(lua_State *L) {
      lua_pushboolean(L, railStation());
      return 1;
    }
    int Block::l_getRailStationTrain(lua_State *L) {
      lua_pushboolean(L, railStationTrain());
      return 1;
    }
    int Block::l_getTextureId(lua_State *L) {
      int which = luaL_checkinteger(L, 1);
      int v = 0;
      if ((which < 0) || (which > 4))
        throw E_OUTOFRANGE("Quad id " + std::to_string(which) + " is invalid");
      switch(which) {
        case 0:
          v = lid;
          break;
        case 1:
          v = top;
          break;
        case 2:
          v = bottom;
          break;
        case 3:
          v = left;
          break;
        case 4:
          v = right;
          break;
        default:
          break;
          // handled above 
      }
      lua_pushinteger(L, v);
      return 1;
    }

    int LMap::l_getBlockAt(lua_State *L) {
      int x, y, z;
      x = luaL_checkinteger(L, 1);
      y = luaL_checkinteger(L, 2);
      z = luaL_checkinteger(L, 3);
      OpenGTA::Map::BlockInfo* bi = getBlockAtNew(x, y, z);
      assert(bi);
      Block* b = static_cast<Block*>(bi);
      Lunar<Block>::push(L, b, false);
      return 1;
    }
    int LMap::l_getNumBlocksAt(lua_State *L) {
      int x, y;
      x = luaL_checkinteger(L, 1);
      y = luaL_checkinteger(L, 2);
      lua_pushinteger(L, getNumBlocksAtNew(x, y));
      return 1;
    }

    const char Block::className[] = "Block";
#define method(name) {#name, &Block::l_##name}
    Lunar<Block>::RegType Block::methods[] = {
      method(getBlockType),
      method(getSlopeType),
      method(getRotation),
      method(getIsFlat),
      method(getLeftOk),
      method(getRightOk),
      method(getUpOk),
      method(getDownOk),
      method(getRailway),
      method(getRailStation),
      method(getRailStationTrain),
      method(getTextureId),

      method(setBlockType),
      method(setRotation),
      method(setSlopeType),
      {0, 0}
    };
#undef method
#define method(name) {#name, &LMap::l_##name}
    const char LMap::className[] = "Map";
    Lunar<LMap>::RegType LMap::methods[] = {
      method(getNumBlocksAt),
      method(getBlockAt),
      {0, 0}
    };


  }
}
