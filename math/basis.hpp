#ifndef GOMEZ_Basis_H
#define GOMEZ_Basis_H

/**
 * Taken from:
 * http://www.gamasutra.com/features/19990702/data_structures_01.htm
 * http://www.gamasutra.com/features/19991018/Gomez_1.htm
 *
 * Both by Miguel Gomez
 */


#include "matrix.hpp"

namespace GomezMath {
// An orthonormal basis with respect to a parent
//
class Basis
{
public:
  Matrix R;
public:
  Basis ()
  {
  }
  Basis (const Vector & v0,
	 const Vector & v1, const Vector & v2):R (v0, v1, v2)
  {
  }
  Basis (const Matrix & m):R (m)
  {
  }
  const Vector & operator [] (long i) const
  {
    return R.C[i];
  }
  const Vector & x () const
  {
    return R.C[0];
  }
  const Vector & y () const
  {
    return R.C[1];
  }
  const Vector & z () const
  {
    return R.C[2];
  }
  const Matrix & basis () const
  {
    return R;
  }
  void basis (const Vector & v0, const Vector & v1, const Vector & v2)
  {
    this->R[0] = v0;
    this->R[1] = v1;
    this->R[2] = v2;
  }
// Right-Handed Rotations
  void rotateAboutX (const Scalar & a)
  {
    if (0 != a)			//don’t rotate by 0
      {
	Vector b1 = this->y () * cos (a) + this->z () * sin (a);
	Vector b2 = -this->y () * sin (a) + this->z () * cos (a);
//set basis
	this->R[1] = b1;
	this->R[2] = b2;
//x is unchanged
      }
  }
  void rotateAboutY (const Scalar & a)
  {
    if (0 != a)			//don’t rotate by 0
      {
	Vector b2 = this->z () * cos (a) + this->x () * sin (a);	//rotate z
	Vector b0 = -this->z () * sin (a) + this->x () * cos (a);	//rotate x
//set basis
	this->R[2] = b2;
	this->R[0] = b0;
//y is unchanged
      }
  }
  void rotateAboutZ (const Scalar & a)
  {
    if (0 != a)			//don’t rotate by 0
      {
//don’t over-write basis before calculation is done
	Vector b0 = this->x () * cos (a) + this->y () * sin (a);	//rotate x
	Vector b1 = -this->x () * sin (a) + this->y () * cos (a);	//rotate y
//set basis
	this->R[0] = b0;
	this->R[1] = b1;
//z is unchanged
      }
  }
//rotate the basis about the unit axis u by theta (radians)
  void rotate (const Scalar & theta, const Vector & u);
//rotate, length of da is theta, unit direction of da is u
  void rotate (const Vector & da);

// Transformations
  const Vector transformVectorToLocal (const Vector & v) const
  {
    return Vector (R.C[0].dot (v), R.C[1].dot (v), R.C[2].dot (v));
  }
  const Point transformVectorToParent (const Vector & v) const
  {
    return R.C[0] * v.x + R.C[1] * v.y + R.C[2] * v.z;
  }
};

}
#endif
