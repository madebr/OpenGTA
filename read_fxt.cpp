/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This file contains code derived from information copyrighted by       *
* DMA Design. It may not be used in a commercial product.               *
*                                                                       *
* See license.txt for details.                                          *
*                                                                       *
* This notice may not be removed or altered.                            *
************************************************************************/
#include <stdio.h>
#include <iostream>
#include "opengta.h"
#include "m_exceptions.h"
#include "log.h"

namespace OpenGTA {
  MessageDB::MessageDB() {
    load("ENGLISH.FXT");
    _error = "ERROR";
  }
  MessageDB::MessageDB(const std::string &file) {
    load(file);
  }
  MessageDB::~MessageDB() {
    messages.clear();
  }
  void MessageDB::load(const std::string &file) {
    PHYSFS_file* f = PHYSFS_openRead(file.c_str());
    if (f == NULL) {
      std::string f2(file);
      transform(f2.begin(), f2.end(), f2.begin(), tolower);
      f = PHYSFS_openRead(f2.c_str());
    }
    if (f == NULL) {
      throw E_FILENOTFOUND(file);
    }
    
    messages.clear();
    
    unsigned char v;
    int c = -1;
    unsigned char addVal = 0;
    char buff[200];
    int i = 0;
    std::string tmp;
    while (!PHYSFS_eof(f)) {
      PHYSFS_read(f, static_cast<void*>(&v), 1, 1);

      /* thanks to: Michael Mendelsohn
       * http://gta.mendelsohn.de/
       */

      v--; // decode: decrease by one
      c++; // helper: count bytes read
      if (c <= 7) // polyalphabetic code for the first 8 bytes
        v -= (0x63 << c);
      /* another twist: skip and add 64 (minus decoding) to next */
      if (v == 195) {
        addVal = 64;
      }
      else {
        v += addVal;
        addVal = 0;
        if (v == '[') {
          i = 0;
        }
        else if (v == ']') {
          buff[i] = 0x00;
          tmp = std::string(buff);
          i = 0;
        }
        else if (v == 0x00) {
          buff[i] = 0x00;
          if (tmp.length() > 0)
            messages[tmp] = std::string(buff);
          //std::cout << tmp << " : " << buff << std::endl;
          /*else
            std::cout << "Skipping: " << tmp << ": " << buff << std::endl;*/
        }
        else {
          buff[i] = v;
          i++;
          if (i>199)
            i=0;
        }

      }
    }
    PHYSFS_close(f);
  }

  const std::string& MessageDB::getText(const char* id) {
    std::map<std::string, std::string>::iterator i = messages.find(std::string(id));
    if (i == messages.end()) {
      ERROR << "string lookup failed for key: " << id << std::endl;
      return _error;
    }
    return i->second;
  }
  
  const std::string& MessageDB::getText(const std::string &id) {
    std::map<std::string, std::string>::iterator i = messages.find(id);
    if (i == messages.end()) {
      ERROR << "string lookup failed for key: " << id << std::endl;
      return _error;
    }
    return i->second;
  }
  
  const std::string& MessageDB::getText(const uint32_t id) {
    char tmp[10];
    snprintf(reinterpret_cast<char*>(&tmp), 10, "%i", id);
    std::map<std::string, std::string>::iterator i = messages.find(std::string(tmp));
    if (i == messages.end()) {
      ERROR << "string lookup failed for key: " << id << std::endl;
      return _error;
    }
    return i->second;
  }

}

#if FXT_TEST

int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  PHYSFS_addToSearchPath("gtadata.zip", 1);
  OpenGTA::MessageDB* strings = new OpenGTA::MessageDB();
  std::cout << strings->getText(1001) << std::endl;
  
  delete strings;
  PHYSFS_deinit();
}

#endif
