/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/
#ifndef UTIL_CELLITERATOR_H
#define UTIL_CELLITERATOR_H
#include <cassert>
#include "opengta.h"
#include "dataholder.h"
#include "math3d.h"
#include "SDL_video.h"

namespace Util {
  float distance(const Vector3D & p1, const Vector3D & p2);
  float xz_angle(const Vector3D & from, const Vector3D & to);
  float xz_turn_angle(const Vector3D & from, const Vector3D & to);

  class CellIterator {
    public:
      CellIterator(const Vector3D & p) :
        x(int(floor(p.x))), y(int(floor(p.z))), z(int(floor(p.y))),
        mapRef(OpenGTA::ActiveMap::Instance().get()) {}

      CellIterator(OpenGTA::Map & map, int _x, int _y, int _z) :
        x(_x), y(_y), z(_z), mapRef(map) {}

      CellIterator(const CellIterator & o) :
        x(o.x), y(o.y), z(o.z), mapRef(o.mapRef) {}

      bool isValid() const;
      int  distance(const CellIterator & o) const;

      bool operator == (const CellIterator & o) const {
        return (x == o.x && y == o.y && z == o.z);
      }

      CellIterator operator = (const CellIterator & o) {
        mapRef = o.mapRef;
        x = o.x;
        y = o.y;
        z = o.z;
        return *this;
      }

      CellIterator left() const {
        CellIterator p(*this);
        p.x -= 1;
        return p;
      }
      CellIterator right() const {
        CellIterator p(*this);
        p.x += 1;
        return p;
      }
      CellIterator top() const {
        CellIterator p(*this);
        p.y += 1;
        return p;
      }
      CellIterator bottom() const {
        CellIterator p(*this);
        p.y -= 1;
        return p;
      }
      CellIterator up() const {
        CellIterator p(*this);
        p.z += 1;
        return p;
      }
      CellIterator down() const {
        CellIterator p(*this);
        p.z -= 1;
        return p;
      }
      bool isBlockType(uint8_t t) const;
      std::pair<bool, CellIterator> findTypeInCol(uint8_t t) const;
      std::pair<bool, CellIterator> findNeighbourWithType(uint8_t t, float angle_hint);
      int x, y, z;
      OpenGTA::Map::BlockInfo & getBlock() const;
    private:
      OpenGTA::Map & mapRef;
  };

}

#endif
