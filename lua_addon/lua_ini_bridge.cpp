#include <cassert>
#include "lua_ini_bridge.h"
#include "log.h"

namespace OpenGTA { namespace Script {

#define GLOBAL_TABLE(name) lua_newtable(L); lua_setglobal(L, name)

IniScriptBridge::IniScriptBridge(const std::string & file) :
  ScriptParser(file) {
    L = lua_open();
    luaL_openlibs(L);
}

IniScriptBridge::~IniScriptBridge() {
  lua_close(L);
}

void IniScriptBridge::reset() {
  lua_settop(L, 0);
  GLOBAL_TABLE("commands");
  GLOBAL_TABLE("definitions");
  lua_gc(L, LUA_GCCOLLECT, 0);
  lua_settop(L, 0);
}

void IniScriptBridge::loadLevel(PHYSFS_uint32 level) {
  reset();
  ScriptParser::loadLevel(level);
}

void IniScriptBridge::acceptCommand(const char* cmd) {
  char *skip_idx = strchr(cmd, ' ');
  assert(skip_idx);
  *skip_idx = 0;
  int idx = atoi(cmd);
  ++skip_idx;
  //INFO << idx << " " << skip_idx << std::endl;
  lua_settop(L, 0);

  lua_getglobal(L, "commands");
  lua_pushinteger(L, idx);
  lua_pushstring(L, skip_idx);
  lua_settable(L, -3);
  
  lua_settop(L, 0);
}

void IniScriptBridge::acceptDefinition(const char* def) {
  char *skip_idx = strchr(def, ' ');
  assert(skip_idx);
  *skip_idx = 0;
  int idx = atoi(def);
  skip_idx++;
  if (*skip_idx == '1' && *(skip_idx+1) == ' ')
    skip_idx += 2;
  //INFO << idx << " " << skip_idx << std::endl;

  lua_settop(L, 0);

  lua_getglobal(L, "definitions");
  lua_pushinteger(L, idx);
  lua_pushstring(L, skip_idx);
  lua_settable(L, -3);
  
  lua_settop(L, 0);
}

}}

#if 0
void on_exit() {
   PHYSFS_deinit();
}

int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  atexit(on_exit);
  OpenGTA::Script::IniScriptBridge p(argv[1]);
  p.loadLevel(atoi(argv[2]));

  if (luaL_loadfile(p.L, argv[3]) || lua_pcall(p.L, 0, 0, 0))
        ERROR << lua_tostring(p.L, -1);

  return 0;
}
#endif
