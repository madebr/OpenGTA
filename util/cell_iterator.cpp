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
  
  bool CellIterator::isValid() const {
    if (x < 0 || x > 255)
      return false;
    if (y < 0 || y > 255)
      return false;
    if (z < 0)
      return false;
    return (z < mapRef.getNumBlocksAtNew(x, y));
  }

  OpenGTA::Map::BlockInfo & CellIterator::getBlock() {
    assert(isValid());
    return *mapRef.getBlockAtNew(x, y, z);
  }

#define IABS(v) ((v > 0) ? v : -v)
  int CellIterator::distance(const CellIterator & o) const {
    return (IABS(x - o.x) + IABS(y - o.y) + IABS(z - o.z));
  }

}
