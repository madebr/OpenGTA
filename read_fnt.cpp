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
#include <iostream>
#include <cassert>
#include "file_helper.h"
#include "opengta.h"
#include "m_exceptions.h"
#include "log.h"
#include "string_helpers.h"

namespace OpenGTA {
  Font::Font(const std::string &file) {
    PHYSFS_file *fd = Util::FileHelper::OpenReadVFS(file);
    readHeader(fd);
    int ww = 0;
    int lw = 0;
    for (uint8_t i = 0; i < numChars; i++) {
      Character * ch = new Character(fd, charHeight);
      ww += ch->width;
      if (ch->width > lw)
        lw = ch->width;
      chars.push_back(ch);
    }
    INFO << "total width " << ww << " largest width " << lw << std::endl;
    palette.loadFromFile(fd);
    PHYSFS_close(fd);
    size_t ih = charHeight;
    while (ww > 1024) {
      ih *= 2;
      ww /= 2;
    }
    workBuffer = new unsigned char[lw*charHeight*4];
    loadMapping(file);
  }
  Font::~Font() {
    std::vector<Character*>::iterator i = chars.begin();
    while(i != chars.end()) {
      delete *i;
      i++;
    }
    chars.clear();
    delete [] workBuffer;
  }
  void Font::readHeader(PHYSFS_file *fd) {
    PHYSFS_readBytes(fd, static_cast<void*>(&numChars), 1);
    PHYSFS_readBytes(fd, static_cast<void*>(&charHeight), 1);
    INFO << "Font contains " << int(numChars) << 
      " characters of height " << int(charHeight) << std::endl;
  }
  void Font::addMapping(char c, size_t num) {
    mapping[c] = num;
  }
  Font::Character* Font::getCharById(size_t num) {
    return chars[num];
  }
  size_t Font::getIdByChar(const char c) {
    std::map<char, size_t>::iterator i = mapping.find(c);
    if (i == mapping.end())
      return 0;
    else
      return i->second;
  }
  uint8_t Font::getMoveWidth(const char c) {
    std::map<char, size_t>::iterator i = mapping.find(c);
    if (i == mapping.end()) {
      return chars[0]->width;
    }
    return chars[i->second]->width;
  }
    
  unsigned char* Font::getCharacterBitmap(size_t num, unsigned int *width, unsigned int *height) {
    unsigned int len = chars[num]->width;
    len *= charHeight;
    palette.apply(len, chars[num]->rawData, workBuffer, true);
    if (width != NULL)
      *width = chars[num]->width;
    if (height != NULL)
      *height = charHeight;
    return workBuffer;
    /*
    unsigned int glwidth = 1;
    unsigned int glheight = 1;

    while(glwidth < chars[num]->width)
      glwidth <<= 1;

    while(glheight < charHeight)
      glheight <<= 1;
    unsigned char *res = new unsigned char[glwidth*glheight*4];
    */
  }
  void Font::dumpAs(const char* filename, size_t id) {
    unsigned int len = chars[id]->width;
    len *= charHeight;
    palette.apply(len, chars[id]->rawData, workBuffer, true);
#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define rmask 0xff000000 
#define gmask 0x00ff0000 
#define bmask 0x0000ff00 
#define amask 0x000000ff
#else 
#define rmask 0x000000ff 
#define gmask 0x0000ff00
#define bmask 0x00ff0000 
#define amask 0xff000000 
#endif 
  SDL_Surface* s = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 
      chars[id]->width, charHeight, 32, rmask, gmask, bmask, amask);
  SDL_LockSurface(s);
  unsigned char* dst = static_cast<unsigned char*>(s->pixels);
  unsigned char * rp = workBuffer;
  for (unsigned int i=0; i<len; i++) {
    *dst = *rp; ++dst;++rp;
    *dst = *rp; ++dst;++rp;
    *dst = *rp; ++dst;++rp;
    //*dst = 0xff; ++dst;
    *dst = *rp; ++dst;++rp;
  }
  SDL_UnlockSurface(s);
  SDL_SaveBMP(s, filename);
  SDL_FreeSurface(s);
  }
  Font::Character::Character(PHYSFS_file *fd, uint8_t height) {
    PHYSFS_readBytes(fd, static_cast<void*>(&width), 1);
    size_t c = size_t(width) * size_t(height);
    //std::cout <<"width " << int(width) << " going to read " << c << " bytes" << std::endl;
    rawData = new uint8_t[c];
    PHYSFS_readBytes(fd, static_cast<void*>(rawData), c);
  }
  Font::Character::~Character() {
    delete [] rawData;
  }
  
  void Font::loadMapping(const std::string & name) {
    std::string name2 { Util::string_lower(name) };
#define chr(n) ((char)(n))
    if (name2.find("big1.fon") != std::string::npos) {
      INFO << "found mapping: big1.fon - " << name << std::endl;
      addMapping('!', 0);
      addMapping('-', 12);
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 192; j < 195; j++) {
        addMapping(chr(j), j - 97);
      }
      addMapping(196, 98);
      addMapping(198, 99);
      addMapping(199, 100);
      for (int j=200; j < 208; j++)
        addMapping(j, j - 99);
      for (int j=210; j < 213; j++)
        addMapping(j, j - 101);
      addMapping(214, 112);
      for (int j=217; j < 221; j++)
        addMapping(j, j - 104);
      addMapping(223, 117);
    }
    else if ((name2.find("pager1.fon") != std::string::npos) ||
      (name2.find("pager2.fon") != std::string::npos)) {
      addMapping('!', 0);
      addMapping('"', 1);
      addMapping('$', 3);
      addMapping('\'', 6);
      addMapping('(', 7);
      addMapping(')', 8);
      addMapping(',', 11);
      addMapping('.', 13);
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      addMapping(':', 25);
      addMapping(';', 26);
      addMapping('<', 27);
      addMapping('>', 29);
      addMapping('?', 30);
      addMapping('_', 62);
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 192; j < 195; j++) {
        addMapping(chr(j), j - 97);
      }
      addMapping(196, 98);
      addMapping(198, 99);
      addMapping(199, 100);
      for (int j=200; j < 208; j++)
        addMapping(j, j - 99);
      for (int j=210; j < 213; j++)
        addMapping(j, j - 101);
      addMapping(214, 112);
      for (int j=217; j < 221; j++)
        addMapping(j, j - 104);
      addMapping(223, 117);
    }
    else if (name2.find("street1.fon") != std::string::npos) {
      INFO << "found mapping: street1.fon - " << name << std::endl;
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 97; j < 123; j++) {
        addMapping(chr(j), j - 33);
      }
      WARN << "incomplete mapping" << std::endl;
    }
    else if ((name2.find("m_mmiss.fon") != std::string::npos)) {
      addMapping('!', 0);
      addMapping('"', 1);
      addMapping('#', 2);
      addMapping('$', 3);
      addMapping('\'', 6);
      addMapping('(', 7);
      addMapping(')', 8);
      addMapping('+', 10);
      addMapping(',', 11);
      addMapping('.', 13);
      addMapping('/', 14);
      addMapping(':', 25);
      addMapping(';', 26);
      addMapping('<', 27);
      addMapping('=', 28);
      addMapping('>', 29);
      addMapping('?', 30);
      addMapping('\\', 59);
      addMapping('[', 58);
      addMapping(']', 60);
      addMapping('|', 91);
      addMapping('~', 93);
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 97; j < 123; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 192; j < 195; j++) {
        addMapping(chr(j), j - 97);
      }
      // incomplete
    }
    else if ((name2.find("f_mtext.fon") != std::string::npos)) {
      addMapping('!', 0);
      addMapping('"', 1);
      addMapping('#', 2);
      addMapping('$', 3);
      addMapping('%', 4);
      addMapping('\'', 6);
      addMapping('(', 7);
      addMapping(')', 8);
      addMapping(169, 9); // copyright
      addMapping(',', 11);
      addMapping('-', 12);
      addMapping('.', 13);
      addMapping('/', 14);
      addMapping(':', 25);
      addMapping(';', 26);
      addMapping('<', 27);
      addMapping('=', 28);
      addMapping('>', 29);
      addMapping('?', 30);
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      addMapping('\\', 59);
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 97; j < 123; j++) {
        addMapping(chr(j), j - 33);
      }
      // incomplete

    }
    else if ((name2.find("f_mhead.fon") != std::string::npos)) {
      addMapping('!', 0);
      addMapping('"', 1);
      addMapping('#', 2);
      addMapping('$', 3);
      addMapping('\'', 6);
      addMapping('(', 7);
      addMapping(')', 8);
      addMapping(',', 11);
      addMapping('.', 13);
      addMapping('/', 14);
      addMapping(':', 25);
      addMapping(';', 26);
      addMapping('<', 27);
      addMapping('>', 29);
      addMapping('?', 30);
      addMapping('\\', 59);
      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 97; j < 123; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 192; j < 195; j++) {
        addMapping(chr(j), j - 97);
      }
      addMapping(196, 98);
      addMapping(198, 99);
      addMapping(199, 100);
      for (int j=200; j < 208; j++)
        addMapping(j, j - 99);
      for (int j=210; j < 213; j++)
        addMapping(j, j - 101);
      addMapping(214, 112);
      for (int j=217; j < 221; j++)
        addMapping(j, j - 104);
      addMapping(223, 117);
      for (int j=224; j < 227; j++)
        addMapping(j, j - 106);
      addMapping(228, 121);
      for (int j=230; j < 240; j++)
        addMapping(j, j - 108);
      for (int j=242; j < 245; j++)
        addMapping(j, j - 110);
      addMapping(246, 135);
      for (int j=249; j < 253; j++)
        addMapping(j, j - 113);

    }
    else if ((name2.find("sub1.fon") != std::string::npos) ||
      (name2.find("sub2.fon") != std::string::npos)) {
      addMapping('!', 0);
      addMapping('"', 1);
      addMapping('$', 3);
      addMapping('\'', 6); // ´
      addMapping('(', 7);
      addMapping(')', 8);
      addMapping(',', 11);
      addMapping('-', 12); // not in street1/2
      addMapping('.', 13);
      addMapping('/', 14);
      addMapping(':', 25);
      addMapping(';', 26);
      addMapping('<', 27);
      addMapping('>', 29);
      addMapping('?', 30);

      for (int j = 65; j < 91; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 97; j < 123; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 33);
      }
      for (int j = 192; j < 195; j++) {
        addMapping(chr(j), j - 97);
      }
      addMapping(196, 98);
      addMapping(198, 99);
      addMapping(199, 100);
      for (int j=200; j < 208; j++)
        addMapping(j, j - 99);
      for (int j=210; j < 213; j++)
        addMapping(j, j - 101);
      addMapping(214, 112);
      for (int j=217; j < 221; j++)
        addMapping(j, j - 104);
      addMapping(223, 117);
      for (int j=224; j < 227; j++)
        addMapping(j, j - 106);
      addMapping(228, 121);
      for (int j=230; j < 240; j++)
        addMapping(j, j - 108);
      for (int j=242; j < 245; j++)
        addMapping(j, j - 110);
      addMapping(246, 135);
      for (int j=249; j < 253; j++)
        addMapping(j, j - 113);
    }
    else if ((name2.find("score1.fon") != std::string::npos)||
      (name2.find("score2.fon") != std::string::npos) ||
      (name2.find("score8.fon") != std::string::npos)) {
      for (int j = 48; j < 58; j++) {
        addMapping(chr(j), j - 48);
      }
    }
    else {
      ERROR << "mapping for font " << name << " is not known" << std::endl;
    }
  }
}

#ifdef FONT_DUMP_TOOL
#include <stdlib.h>
void do_exit() {
  PHYSFS_deinit();
}
int main(int argc, char* argv[]) {
  PHYSFS_init(argv[0]);
  atexit(do_exit);
  std::cout << "Physfs-Base: " << PHYSFS_getBaseDir() << std::endl;
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  std::cout << "Has: " << argv[1] << " : " << PHYSFS_exists(argv[1]) << std::endl;
  OpenGTA::Font a(argv[1]);
  a.dumpAs("out.bmp", atoi(argv[2]));
  return 0;
}
#endif
