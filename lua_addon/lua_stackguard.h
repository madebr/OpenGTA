#ifndef LUA_STACK_GUARD_H
#define LUA_STACK_GUARD_H

#include "lua.hpp"

namespace Util {

  class LuaStackguard {
    public:
      LuaStackguard(const char* f, int l, lua_State *L);
      ~LuaStackguard();
    private:
      int m_top;
      lua_State* m_state;
      // line number and filename where this instance was created
      int i_line;
      const char* i_file;
  };

#define LGUARD(L) LuaStackguard guard(__FILE__, __LINE__, L)

}
#endif
