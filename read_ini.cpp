/************************************************************************
 * Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
 *                                                                       *
 * This file contains code derived from information copyrighted by       *
 * DMA Design. It may not be used in a commercial product.               *
 *                                                                       *
 * See license.txt for details.                                          *
 *                                                                       *
 * This notice may not be removed or altered.                            *
 ************************************************************************/
#include <iostream>
#include <string>
#include <map>
#include <stdlib.h>
#include <physfs.h>

namespace OpenGTA {

  class ScriptParser {
    public:
      ScriptParser(const std::string &file);
      ~ScriptParser();
      void loadLevel(PHYSFS_uint32 level);
    private:
      typedef std::map<PHYSFS_uint32, PHYSFS_sint64> LevelMapType;
      LevelMapType levels;
      PHYSFS_file* fd;
  };

  ScriptParser::ScriptParser(const std::string &file) {
    fd = PHYSFS_openRead(file.c_str());
    if (!fd) {
      std::cerr << "Error: could not open file " << file << " for reading!" << std::endl;
    }
    else {
      std::cout << "* Loading script " << file << " ... ";
      unsigned char v;
      unsigned char m = 0;
      char buffer[10];
      char* b = reinterpret_cast<char*>(&buffer);
      while(!PHYSFS_eof(fd)) {
        PHYSFS_read(fd, static_cast<void*>(&v), 1, 1);
        if (m) {
          if (v != ']') {
            *b = v;
            b++;
          }
          else {
            m = 0;
            *b = 0x00;
            b = reinterpret_cast<char*>(&buffer);
            levels[static_cast<PHYSFS_uint32>(atoi(buffer))] = PHYSFS_tell(fd) + 1;
          }
        }
        if (v == '[') {
          m = 1;
        }
      }
      std::cout << int(levels.size()) << " sections indexed" << std::endl;
    }
  }

  ScriptParser::~ScriptParser() {
    if (fd != NULL)
      PHYSFS_close(fd);
    levels.clear();
  }

  void ScriptParser::loadLevel(PHYSFS_uint32 level) {
    LevelMapType::iterator i = levels.find(level);
    if (i == levels.end()) {
      std::cerr << "not a valid level: " << level << std::endl;
      return;
    }
    PHYSFS_seek(fd, i->second);
    char buffer[256];
    PHYSFS_uint16 read_bytes = 255;
    PHYSFS_uint16 offset = 0;
    while(!PHYSFS_eof(fd)) {
      memset(buffer+offset, 0, read_bytes+1);
      PHYSFS_read(fd, buffer + offset, 1, read_bytes);
      char* line_start = buffer;
      while (1) {
        char* line_end = strchr(line_start, '\r');
        if (line_start && line_end) { 
          *line_end = 0;
          if (strlen(line_start) > 0) {
            std::cout <<"["<< line_start << "]" << strlen(line_start)<<std::endl;
          }
          line_start = line_end + 1;
          if (*line_start == '\n')
            ++line_start;
        }
        else
          break;
      }
      typedef unsigned int uint32;
      //std::cout << uint32(line_start) - uint32(buffer) << std::endl; 
      PHYSFS_uint32 begin_rest = PHYSFS_uint32(line_start) - PHYSFS_uint32(buffer);
      offset = 255 - begin_rest;
      memmove(buffer, &buffer[begin_rest], 255 - begin_rest);
      read_bytes = 255 - offset;
      //std::cout << buffer << std::endl;
    }
  }

}

#if 0
void on_exit() {
   PHYSFS_deinit();
}

int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
  atexit(on_exit);
  OpenGTA::ScriptParser p(argv[1]);
  p.loadLevel(atoi(argv[2]));
  return 0;
}

#endif
