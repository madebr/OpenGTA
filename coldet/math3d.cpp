/*   ColDet - C++ 3D Collision Detection Library
 *   Copyright (C) 2000   Amir Geva
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA  02111-1307, USA.
 *
 * Any comments, questions and bug reports send to:
 *   photon@photoneffect.com
 *
 * Or visit the home page: http://photoneffect.com/coldet/
 */
#include "sysdep.h"
#include "math3d.h"

const Vector3D Vector3D::Zero(0.0f,0.0f,0.0f);
const Matrix3D Matrix3D::Identity(1.0f,0.0f,0.0f,0.0f,
                                  0.0f,1.0f,0.0f,0.0f,
                                  0.0f,0.0f,1.0f,0.0f,
                                  0.0f,0.0f,0.0f,1.0f);


inline float
MINOR(const Matrix3D& m, const int r0, const int r1, const int r2, const int c0, const int c1, const int c2)
{
   return m(r0,c0) * (m(r1,c1) * m(r2,c2) - m(r2,c1) * m(r1,c2)) -
          m(r0,c1) * (m(r1,c0) * m(r2,c2) - m(r2,c0) * m(r1,c2)) +
          m(r0,c2) * (m(r1,c0) * m(r2,c1) - m(r2,c0) * m(r1,c1));
}


Matrix3D
Matrix3D::Adjoint() const
{
   return Matrix3D( MINOR(*this, 1, 2, 3, 1, 2, 3),
                   -MINOR(*this, 0, 2, 3, 1, 2, 3),
                    MINOR(*this, 0, 1, 3, 1, 2, 3),
                   -MINOR(*this, 0, 1, 2, 1, 2, 3),

                   -MINOR(*this, 1, 2, 3, 0, 2, 3),
                    MINOR(*this, 0, 2, 3, 0, 2, 3),
                   -MINOR(*this, 0, 1, 3, 0, 2, 3),
                    MINOR(*this, 0, 1, 2, 0, 2, 3),

                    MINOR(*this, 1, 2, 3, 0, 1, 3),
                   -MINOR(*this, 0, 2, 3, 0, 1, 3),
                    MINOR(*this, 0, 1, 3, 0, 1, 3),
                   -MINOR(*this, 0, 1, 2, 0, 1, 3),

                   -MINOR(*this, 1, 2, 3, 0, 1, 2),
                    MINOR(*this, 0, 2, 3, 0, 1, 2),
                   -MINOR(*this, 0, 1, 3, 0, 1, 2),
                    MINOR(*this, 0, 1, 2, 0, 1, 2));
}


float
Matrix3D::Determinant() const
{
   return m[0][0] * MINOR(*this, 1, 2, 3, 1, 2, 3) -
          m[0][1] * MINOR(*this, 1, 2, 3, 0, 2, 3) +
          m[0][2] * MINOR(*this, 1, 2, 3, 0, 1, 3) -
          m[0][3] * MINOR(*this, 1, 2, 3, 0, 1, 2);
}

Matrix3D 
Matrix3D::Inverse() const
{
   return (1.0f / Determinant()) * Adjoint();
}

Vector3D Matrix3D::GetTranslate() const
{
  return Vector3D(m[3][0], m[3][1], m[3][2]);
}

void Matrix3D::Translate(const Vector3D & v)
{
  m[3][0] += v.x * m[0][0] + v.y * m[1][0] + v.z * m[2][0];
  m[3][1] += v.x * m[0][1] + v.y * m[1][1] + v.z * m[2][1];
  m[3][2] += v.x * m[0][2] + v.y * m[1][2] + v.z * m[2][2];
}

void Matrix3D::RotZ(float angle) {
  const float TO_RAD = M_PI / 180.0f;
  m[0][0] =  cosf(angle*TO_RAD);
  m[1][0] =  sinf(angle*TO_RAD);
  m[0][1] =  -sinf(angle*TO_RAD);
  m[1][1] =  cosf(angle*TO_RAD);
}
