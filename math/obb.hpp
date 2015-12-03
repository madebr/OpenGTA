#ifndef GOMEZ_OBB_H
#define GOMEZ_OBB_H

/**
 * Taken from:
 * http://www.gamasutra.com/features/19990702/data_structures_01.htm
 * http://www.gamasutra.com/features/19991018/Gomez_1.htm
 *
 * Both by Miguel Gomez
 */

#include "coord_frame.hpp"

namespace GomezMath {
  class OBB : public CoordFrame
  {
    public:
      Vector E;			//extents
      OBB (const Vector & e) : E (e)
      {
      }
      OBB (const Vector & e, const Vector & origin, 
        const Vector & v0, const Vector & v1, const Vector & v2) :
          CoordFrame(origin, v0, v1, v2), E(e)
      {
      }
  };

  const bool OBBOverlap (
      //A
      Vector & a,	//extents
      Vector & Pa,	//position
      Vector * A,	//orthonormal basis
      //B
      Vector & b,	//extents
      Vector & Pb,	//position
      Vector * B	//orthonormal basis
      );

}

#endif
