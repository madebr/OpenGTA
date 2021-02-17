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
#include "cell_iterator.h"
#include "log.h"

namespace Util {
  float distance(const Vector3D & p1, const Vector3D & p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    float dz = p1.z - p2.z;
    return sqrt(dx * dx + dy * dy + dz * dz);
  }
  float xz_angle(const Vector3D & from, const Vector3D & to) {
    Vector3D rel_to(to);
    rel_to = rel_to - from;
    double res = atan(rel_to.x / rel_to.z) * 180.0f / M_PI;
    if (rel_to.z < 0)
      return 180.0f + res;
    if (rel_to.z >= 0 && rel_to.x < 0)
      return 360.0f + res;
    return res;
  }
  float xz_turn_angle(const Vector3D & from, const Vector3D & to) {
    Vector3D rel_to(to);
    rel_to = rel_to - from;
    double res = atan(rel_to.x / rel_to.z) * 180.0f / M_PI;
    return res;
  }

  bool CellIterator::isValid() const {
    if (x < 0 || x > 255)
      return false;
    if (y < 0 || y > 255)
      return false;
    if (z < 0)
      return false;
    return (z < mapRef.getNumBlocksAtNew(x, y));
  }

  OpenGTA::Map::BlockInfo & CellIterator::getBlock() const {
    assert(isValid());
    return *mapRef.getBlockAtNew(x, y, z);
  }

#define IABS(v) ((v > 0) ? v : -v)
  int CellIterator::distance(const CellIterator & o) const {
    return (IABS(x - o.x) + IABS(y - o.y) + IABS(z - o.z));
  }

  bool CellIterator::isBlockType(uint8_t t) const {
    if (!isValid())
      return false;
    return (getBlock().blockType() == t);
  }

  std::pair<bool, CellIterator> CellIterator::findTypeInCol(uint8_t t) const {
    if (isBlockType(t))
      return std::make_pair(true, *this);
    CellIterator below = down();
    while (below.isValid()) {
      if (below.isBlockType(t))
        return std::make_pair(true, below);
      below = below.down();
    }
    CellIterator above = up();
    while (above.isValid()) {
      if (above.isBlockType(t))
        return std::make_pair(true, above);
      above = above.up();
    }
    return std::make_pair(false, *this);
  }

  std::pair<bool, CellIterator> CellIterator::findNeighbourWithType(
    uint8_t t, float angle_hint) {
    assert(isValid());

    if (angle_hint >= 315 || angle_hint < 45) {
      std::pair<bool, CellIterator> p = top().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 0  || angle_hint < 90) {
      std::pair<bool, CellIterator> p = top().right().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 45 && angle_hint < 135) {
      std::pair<bool, CellIterator> p = right().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 90 && angle_hint < 180) {
      std::pair<bool, CellIterator> p = right().bottom().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 135 && angle_hint < 225) {
      std::pair<bool, CellIterator> p = bottom().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 180 && angle_hint < 270) {
      std::pair<bool, CellIterator> p = left().bottom().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 225 && angle_hint < 315) {
      std::pair<bool, CellIterator> p = left().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    if (angle_hint >= 270 && angle_hint < 360) {
      std::pair<bool, CellIterator> p = top().left().findTypeInCol(t);
//      if (p.first) INFO << p.second.x << " " << p.second.y << " " << p.second.z << std::endl;
      if (p.first) return p;
    }
    return std::make_pair(false, *this);

  }

}
