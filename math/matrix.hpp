#ifndef GOMEZ_Matrix_H
#define GOMEZ_Matrix_H

#include "vector.hpp"
namespace GomezMath {
/**
 * Taken from:
 * http://www.gamasutra.com/features/19990702/data_structures_01.htm
 * http://www.gamasutra.com/features/19991018/Gomez_1.htm
 *
 * Both by Miguel Gomez
 */


// A 3x3 matrix
//
class Matrix
{
public:
  Vector C[3];			//column vectors
public:
  Matrix ()
  {
//identity matrix
    C[0].x = 1;
    C[1].y = 1;
    C[2].z = 1;
  }
  Matrix (const Vector & c0, const Vector & c1, const Vector & c2)
  {
    C[0] = c0;
    C[1] = c1;
    C[2] = c2;
  }
//index a column, allow assignment
//NOTE: using this index operator along with the vector index
//gives you M[column][row], not the standard M[row][column]
  Vector & operator [](long i)
  {
    return C[i];
  }
//compare
  const bool operator == (const Matrix & m) const
  {
    return C[0] == m.C[0] && C[1] == m.C[1] && C[2] == m.C[2];
  }
  const bool operator != (const Matrix & m) const
  {
    return !(m == *this);
  }
//assign
  const Matrix & operator = (const Matrix & m)
  {
    C[0] = m.C[0];
    C[1] = m.C[1];
    C[2] = m.C[2];
    return *this;
  }
//increment
  const Matrix & operator += (const Matrix & m)
  {
    C[0] += m.C[0];
    C[1] += m.C[1];
    C[2] += m.C[2];
    return *this;
  }
//decrement
  const Matrix & operator -= (const Matrix & m)
  {
    C[0] -= m.C[0];
    C[1] -= m.C[1];
    C[2] -= m.C[2];
    return *this;
  }
//self-multiply by a scalar
  const Matrix & operator *= (const Scalar & s)
  {
    C[0] *= s;
    C[1] *= s;
    C[2] *= s;
    return *this;
  }
//self-multiply by a matrix
  const Matrix & operator *= (const Matrix & m)
  {
//NOTE: don’t change the columns
//in the middle of the operation
    Matrix temp = (*this);
    C[0] = temp * m.C[0];
    C[1] = temp * m.C[1];
    C[2] = temp * m.C[2];
    return *this;
  }
//add
  const Matrix operator + (const Matrix & m) const
  {
    return Matrix (C[0] + m.C[0], C[1] + m.C[1], C[2] + m.C[2]);
  }
//subtract
  const Matrix operator - (const Matrix & m) const
  {
    return Matrix (C[0] - m.C[0], C[1] - m.C[1], C[2] - m.C[2]);
  }
//post-multiply by a scalar
  const Matrix operator * (const Scalar & s) const
  {
    return Matrix (C[0] * s, C[1] * s, C[2] * s);
  }
//pre-multiply by a scalar
  friend inline const Matrix operator * (const Scalar & s, const Matrix & m)
  {
    return m * s;
  }

//post-multiply by a vector
  const Vector operator * (const Vector & v) const
  {
    return (C[0] * v.x + C[1] * v.y + C[2] * v.z);
  }
//pre-multiply by a vector
  inline friend const Vector operator * (const Vector & v, const Matrix & m)
  {
    return Vector (m.C[0].dot (v), m.C[1].dot (v), m.C[2].dot (v));
  }
//post-multiply by a matrix
  const Matrix operator * (const Matrix & m) const
  {
    return Matrix ((*this) * m.C[0], (*this) * m.C[1], (*this) * m.C[2]);
  }
//transpose
  Matrix transpose () const
  {
//turn columns on their side
    return Matrix (Vector (C[0].x, C[1].x, C[2].x),	//column 0
		   Vector (C[0].y, C[1].y, C[2].y),	//column 1
		   Vector (C[0].z, C[1].z, C[2].z)	//column 2
      );
  }
//scalar determinant
  const Scalar determinant () const
  {
//Lang, "Linear Algebra", p. 143
    return C[0].dot (C[1].cross (C[2]));
  }
//matrix inverse
  const Matrix inverse () const;
};

}

#endif
