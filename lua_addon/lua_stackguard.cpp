#include <cassert>
#include "lua_stackguard.h"
#include "log.h"

namespace Util {
  LuaStackguard::LuaStackguard(const char* f, int l, lua_State *L) {
    assert(L);
    m_state = L;
    i_file = f;
    i_line = l;
    m_top = lua_gettop(m_state);
  }

  LuaStackguard::~LuaStackguard() {
    int now_top = lua_gettop(m_state);
    if (now_top > m_top) {
      Util::Log::warn(i_file, i_line) << "Stack-balance: " << now_top << " > " << m_top << std::endl;
      lua_settop(m_state, m_top);
    }
  }

}
