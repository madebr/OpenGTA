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
#include <cassert>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "navdata.h"
#include "log.h"
#include "dataholder.h"
#include "m_exceptions.h"

namespace OpenGTA {
  Rect2D::Rect2D() {
    x = y = w = h = 0;
  }

  Rect2D::Rect2D(PHYSFS_uint8 a, PHYSFS_uint8 b, PHYSFS_uint8 c, PHYSFS_uint8 d) {
    x = a;
    y = b;
    w = c;
    h = d;
  }

  bool Rect2D::isInside(PHYSFS_uint8 _x, PHYSFS_uint8 _y) {
    if ((_x >= x ) && (_y >= y) && 
      (PHYSFS_uint16(_x) <= PHYSFS_uint16(x) + w) && 
      (PHYSFS_uint16(_y) <= PHYSFS_uint16(y) + h)) {
      lastSubLocation = subLocation(_x, _y);
      return true;
    }
    return false;
  }

  PHYSFS_uint16 Rect2D::getSize() {
    PHYSFS_uint16 res = w;
    res *= h;
    return res;
  }

  // 0 = central, 1 = north, 2 = south, 4 = east, 8 = west
  PHYSFS_uint8 Rect2D::subLocation(PHYSFS_uint8 _x, PHYSFS_uint8 _y) {
    PHYSFS_uint8 in_x = _x - x; // offset in rect; assume: x <= _x
    PHYSFS_uint8 in_y = _y - y;
    float rel_x = float(in_x)/w;
    float rel_y = float(in_y)/h;
    PHYSFS_uint8 res = 0;
#define ONE_THIRD 1.0f/3.0f
#define TWO_THIRDS 2.0f/3.0f
    if (rel_x <= ONE_THIRD)
      // INFO << "west" << std::endl;
      res |= 8;
    else if (rel_x >= TWO_THIRDS)
      //INFO << "east" << std::endl;
      res |= 4;
    if (rel_y <= ONE_THIRD)
      res |= 1;
      //INFO << "north" << std::endl;
    else if (rel_y >= TWO_THIRDS)
      res |= 2;
      //INFO << "south" << std::endl;
    return res;
  }

  NavData::Sector::Sector(PHYSFS_file* fd) : Rect2D(), sam(0), name("") {
    sam = 0;
    isADummy = 0;
    assert(fd);
    //memset(name2, 0, 30);
    PHYSFS_readBytes(fd, static_cast<void*>(&x), 1);
    PHYSFS_readBytes(fd, static_cast<void*>(&y), 1);
    PHYSFS_readBytes(fd, static_cast<void*>(&w), 1);
    PHYSFS_readBytes(fd, static_cast<void*>(&h), 1);
    PHYSFS_readBytes(fd, static_cast<void*>(&sam), 1);
    // seek over the name embedded in the mapfile; use sample-num to
    // lookup in msg-db
    //PHYSFS_read(fd, static_cast<void*>(&name2), 30, 1);
    PHYSFS_seek(fd, PHYSFS_tell(fd) + 30);
  }

  NavData::Sector::Sector() : Rect2D(), sam(0), name("") {
    x = 0;
    y = 0;
    w = 255;
    h = 255;
    sam = 0;
    isADummy = 1;
    //memset(name2, 0, 30);
  }

  std::string NavData::Sector::getFullName() const {
    std::string n;
    if (isADummy)
      return n;
    switch (lastSubLocation) {
    case 0:
      //n.append("Central ");
      n.append(_c);
      break;
    case 1:
      //n.append("North ");
      n.append(_n);
      break;
    case 2:
      //n.append("South ");
      n.append(_s);
      break;
    case 4:
      //n.append("East ");
      n.append(_e);
      break;
    case 5:
      //n.append("Northeast ");
      n.append(_ne);
      break;
    case 6:
      //n.append("Southeast ");
      n.append(_se);
      break;
    case 8:
      //n.append("West ");
      n.append(_w);
      break;
    case 9:
      //n.append("Northwest ");
      n.append(_nw);
      break;
    case 10:
      //n.append("Southwest ");
      n.append(_sw);
      break;
    }

    n.append(" ");
    n.append(name);
    return n;
  }

  std::string NavData::_c;
  std::string NavData::_n;
  std::string NavData::_s;
  std::string NavData::_w;
  std::string NavData::_e;
  std::string NavData::_nw;
  std::string NavData::_ne;
  std::string NavData::_sw;
  std::string NavData::_se;

  NavData::NavData(PHYSFS_uint32 size, PHYSFS_file *fd, const size_t level_num) {
    if (size % 35) {
        throw E_INVALIDFORMAT("Navdata size: " + std::to_string(size)
                              + " % 35 != 0");
        // throw std::string("Invalid NavData size in mapfile");
    }
    PHYSFS_uint32 c = size / 35;
    assert(fd);

    MessageDB & msg = MainMsgLookup::Instance().get();
    _c =msg.getText("c");
    _n = msg.getText("n");
    _s = msg.getText("s");
    _w = msg.getText("w");
    _e = msg.getText("e");
    _nw = msg.getText("nw");
    _ne = msg.getText("ne");
    _sw = msg.getText("sw");
    _se = msg.getText("se");
    for (PHYSFS_uint32 i = 0; i < c; ++i) {
      Sector *sec = new Sector(fd);
      if (sec->getSize() == 0) { // workaround for 'NYC.CMP' (empty sectors)
        delete sec;
        WARN << "skipping zero size sector" << std::endl;
        continue;
      }
      else {
        std::ostringstream os;
        os << std::setfill('0') << std::setw(3) << level_num << "area" << std::setfill('0') <<
          std::setw(3) << int(sec->sam);

        //INFO << i << " " << sec->name2 << std::endl << os.str() << " : " << msg.getText(os.str()) << std::endl;
        sec->name = msg.getText(os.str());

        areas.insert(std::pair<PHYSFS_uint16, Sector*>(sec->getSize(), sec));
      }
    }
    // dummy catch-all sector for gta london maps
    areas.insert(std::pair<PHYSFS_uint16, Sector*>(255*255, new Sector()));

    /*
    std::cout << "map areas (by size)" << std::endl;
    SectorMapType::iterator i = areas.begin();
    while (i != areas.end()) {
      std::cout << " " << i->first << " : " << i->second->name2 << " @ " <<
      int(i->second->x) << "," << int(i->second->y)  << "  " << int(i->second->w) << "x" <<
      int(i->second->h) <<  " sample " << int(i->second->sam) << std::endl;
      ++i;
    }
    */
  }
  NavData::~NavData() {
    clear();
  }

  NavData::Sector* NavData::getSectorAt(PHYSFS_uint8 x, PHYSFS_uint8 y) {
    SectorMapType::iterator it = areas.begin();
    while (it != areas.end()) {
      if (it->second->isInside(x, y))
        return it->second;
      ++it;
    }
    throw E_OUTOFRANGE("Querying invalid sector at " + std::to_string(int(x))
                       + ", " + std::to_string(int(y)));
  }

  void NavData::clear() {
    SectorMapType::iterator it = areas.begin();
    while (it != areas.end()) {
      delete it->second;
      ++it;
    }
    areas.clear();
  }
}

#if 0
int main(int argc, char* argv[]) {
  OpenGTA::Rect2D a(3, 4, 10, 20);
  a.subLocation(atoi(argv[1]), atoi(argv[2]));
}
#endif
