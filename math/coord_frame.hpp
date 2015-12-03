#ifndef GOMEZ_COORDFRAME_H
#define GOMEZ_COORDFRAME_H

/**
 * Taken from:
 * http://www.gamasutra.com/features/19990702/data_structures_01.htm
 * http://www.gamasutra.com/features/19991018/Gomez_1.htm
 *
 * Both by Miguel Gomez
 */


#include "basis.hpp"
namespace GomezMath {

// A coordinate frame (basis and origin) with respect to a parent
//
class CoordFrame:public Basis
{
public:
  Point O;			//this coordinate frame’s origin, relative to its parent frame
public:
  CoordFrame ()
  {
  }
  CoordFrame (const Point & o,
	       const Vector & v0,
	       const Vector & v1, const Vector & v2): Basis (v0, v1, v2), O(o)
  {
  }
  CoordFrame (const Point & o, const Basis & b): Basis (b), O(o)
  {
  }
  const Point & position () const
  {
    return O;
  }
  void position (const Point & p)
  {
    O = p;
  }
  const Point transformPointToLocal (const Point & p) const
  {
//translate to this frame’s origin, then project onto this basis
    return transformVectorToLocal (p - O);
  }
  const Point transformPointToParent (const Point & p) const
  {
//transform the coordinate vector and translate by this origin
    return transformVectorToParent (p) + O;
  }
//translate the origin by the given vector
  void translate (const Vector & v)
  {
    O += v;
  }
};
}

#endif
