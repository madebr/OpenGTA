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
#include "file_helper.h"
#include "opengta.h"
#include "m_exceptions.h"
#include "log.h"

namespace {
std::string format_map(const std::map<std::string, std::string> &m)
{
  std::string ret = "{ ";
  for (const auto& [key, val] : m)
    ret.append("{" + key + ": " + val + "}, ");
  ret.append(" }");
  return ret;
}
}

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
    INFO << "Trying to load file " << file << std::endl;
    PHYSFS_file* f = Util::FileHelper::OpenReadVFS(file);
    
    messages.clear();
    
    unsigned char v;
    int c = -1;
    unsigned char addVal = 0;
    char buff[200];
    int i = 0;
    std::string tmp;
    while (!PHYSFS_eof(f)) {
      PHYSFS_readBytes(f, static_cast<void*>(&v), 1);

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
          INFO << "v == 0, tmp = " << tmp << std::endl;
          buff[i] = 0x00;
          if (tmp.length() > 0)
            messages[tmp] = std::string(buff);
          //std::cout << tmp << " : " << buff << std::endl;
          /*else
            std::cout << "Skipping: " << tmp << ": " << buff << std::endl;*/
#ifdef FXT_TEST
          std::cout << tmp << " : " << buff << std::endl;
#endif
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
        ERROR << "string lookup failed for key: " << id
              << ", map = " << format_map(messages) << std::endl;
        return _error;
    }
    return i->second;
  }
  
  const std::string& MessageDB::getText(const std::string &id) {
    std::map<std::string, std::string>::iterator i = messages.find(id);
    if (i == messages.end()) {
        ERROR << "string lookup failed for key: " << id
              << ", map = " << format_map(messages) << std::endl;
        return _error;
    }
    return i->second;
  }
  
  const std::string& MessageDB::getText(const uint32_t id) {
    char tmp[10];
    snprintf(reinterpret_cast<char*>(&tmp), 10, "%i", id);
    std::map<std::string, std::string>::iterator i = messages.find(std::string(tmp));
    if (i == messages.end()) {
        ERROR << "string lookup failed for key: " << id
              << ", map = " << format_map(messages) << std::endl;
        return _error;
    }
    return i->second;
  }

}

#if FXT_TEST
#include "file_helper.h"

int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  const char* lang = getenv("OGTA_LANG");
  if (!lang)
    lang = getenv("LANG");
  if (!lang)
    lang = "en";
  OpenGTA::MessageDB* strings = new OpenGTA::MessageDB(
    Util::FileHelper::Lang2MsgFilename(lang)
  );
  std::cout << strings->getText(1001) << std::endl;
  
  delete strings;
  PHYSFS_deinit();
}

#endif
