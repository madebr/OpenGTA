#ifndef MATH_PLANE_H
#define MATH_PLANE_H

#include "math3d.h"

namespace Math {

  float intersection_segments(const Vector3D & s1, const Vector3D & s2,
  const Vector3D & l1, const Vector3D & l2, Vector3D & hit);

  struct Plane {
    Plane(const Vector3D & p, const Vector3D & n) :
      pop(p), normal(n) {}
    Plane(const Vector3D & p1, const Vector3D & p2, const Vector3D & p3) :
      pop(p1), normal() {
      const Vector3D pa(p2 - p1);
      const Vector3D pb(p3 - p1);
      normal = CrossProduct(pa, pb).Normalized();
    }
    Vector3D pop;
    Vector3D normal;
    int segmentIntersect(const Vector3D & p1, const Vector3D & p2, Vector3D & p_p);
    float distance(const Vector3D & p);
  };
}
#endif
