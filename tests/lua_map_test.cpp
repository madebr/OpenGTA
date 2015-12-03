#include <string>
#include "lua_vm.h"
#include "opengta.h"
#include "dataholder.h"
#include "file_helper.h"
#include "log.h"

std::string map_filename;
const char* script_file;

void on_exit() {
  SDL_Quit();
  PHYSFS_deinit();
}

void parse_args(int argc, char* argv[]) {
  if (argc != 3) {
    ERROR << "invalid args" << std::endl;
    exit(1);
  }
  map_filename = std::string(argv[1]);
  script_file = argv[2];
}

void run_init(const char* prg_name) {
  PHYSFS_init(prg_name);

  Util::FileHelper & fh = GET_FILE_HELPER;
  if (fh.existsInSystemFS(fh.getBaseDataPath())) {
    PHYSFS_addToSearchPath(GET_FILE_HELPER.getBaseDataPath().c_str(), 1);
  }
  else {
    WARN << "Could not load data-source: " << fh.getBaseDataPath() << std::endl;
  }
  
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);

}

void run_main() {
  OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVMHolder::Instance();

  OpenGTA::MainMsgHolder::Instance().load("ENGLISH.FXT");
  OpenGTA::MapHolder::Instance().load(map_filename);
  OpenGTA::Map & loadedMap = OpenGTA::MapHolder::Instance().get();
  vm.setMap(loadedMap);

  vm.runFile(script_file);
}
