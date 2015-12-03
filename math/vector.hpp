#ifndef GOMEZ_Vector_H
#define GOMEZ_Vector_H
#include <cmath>
/**
 * Taken from:
 * http://www.gamasutra.com/features/19990702/data_structures_01.htm
 * http://www.gamasutra.com/features/19991018/Gomez_1.htm
 *
 * Both by Miguel Gomez
 */

namespace GomezMath {

// A floating point number
//
typedef float Scalar;

//
// A 3D vector
//
class Vector
{
public:
  Scalar x, y, z;		//x,y,z coordinates
public:
  Vector ():x (0), y (0), z (0)
  {
  }
  Vector (const Scalar & a, const Scalar & b, const Scalar & c):x (a), y (b),
    z (c)
  {
  }
//index a component
//NOTE: returning a reference allows
//you to assign the indexed element
  Scalar & operator [](const long i)
  {
    return *((&x) + i);
  }
//compare
  const bool operator == (const Vector & v) const
  {
    return (v.x == x && v.y == y && v.z == z);
  }
  const bool operator != (const Vector & v) const
  {
    return !(v == *this);
  }
//negate
  const Vector operator - () const
  {
    return Vector (-x, -y, -z);
  }
//assign
  const Vector & operator = (const Vector & v)
  {
    x = v.x;
    y = v.y;
    z = v.z;
    return *this;
  }
//increment
  const Vector & operator += (const Vector & v)
  {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
//decrement
  const Vector & operator -= (const Vector & v)
  {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
//self-multiply
  const Vector & operator *= (const Scalar & s)
  {
    x *= s;
    y *= s;
    z *= s;
    return *this;
  }
//self-divide
  const Vector & operator /= (const Scalar & s)
  {
    const Scalar r = 1 / s;
    x *= r;
    y *= r;
    z *= r;
    return *this;
  }
//add
  const Vector operator + (const Vector & v) const
  {
    return Vector (x + v.x, y + v.y, z + v.z);
  }
//subtract
  const Vector operator - (const Vector & v) const
  {
    return Vector (x - v.x, y - v.y, z - v.z);
  }
//post-multiply by a scalar
  const Vector operator * (const Scalar & s) const
  {
    return Vector (x * s, y * s, z * s);
  }
//pre-multiply by a scalar
  friend inline const Vector operator * (const Scalar & s, const Vector & v)
  {
    return v * s;
  }
//divide
  const Vector operator / (Scalar s) const
  {
    s = 1 / s;
    return Vector (s * x, s * y, s * z);
  }
//cross product
  const Vector cross (const Vector & v) const
  {
//Davis, Snider, "Introduction to Vector Analysis", p. 44
    return Vector (y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }
//scalar dot product
  const Scalar dot (const Vector & v) const
  {
    return x * v.x + y * v.y + z * v.z;
  }
//length
  const Scalar length () const
  {
    return (Scalar) sqrt ((double) this->dot (*this));
  }
//unit vector
  const Vector unit () const
  {
    return (*this) / length ();
  }
//make this a unit vector
  void normalize ()
  {
    (*this) /= length ();
  }
//equal within an error ‘e’
  const bool nearlyEquals (const Vector & v, const Scalar e) const
  {
    return fabs (x - v.x) < e && fabs (y - v.y) < e && fabs (z - v.z) < e;
  }
};
//
// A 3D position
//
typedef Vector Point;

}
#endif
