#include "plane.h"
#include "log.h"

namespace Math {

  // see: http://local.wasp.uwa.edu.au/~pbourke/geometry/lineline2d/example.cpp
  float intersection_segments(const Vector3D & s1, const Vector3D & s2,
      const Vector3D & l1, const Vector3D & l2, Vector3D & hit) {
      float denom = (l2.z - l1.z) * (s2.x - s1.x) -
        (l2.x - l1.x) * (s2.z - s1.z);

      float d_a = (l2.x - l1.x) * (s1.z - l1.z) - 
        (l2.z - l1.z) * (s1.x - l1.x);

      float d_b = (s2.x - s1.x) * (s1.z - l1.z) -
        (s2.z - s1.z) * (s1.x - l1.x);

      float ua = d_a / denom; // ratio on s1 -> s2
      float ub = d_b / denom; // ratio on l1 -> l2

      if (denom == 0.0f) {
        /*
        if (d_a == 0.0f && d_b == 0.0f)
          INFO << "COINCIDENT" << std::endl;
        else
          INFO << "PARALLEL" << std::endl;
        */
        return -1.0f;
      }

      if (ua >= 0.0f && ua <= 1.0f && ub >= 0.0f && ub <= 1.0f) {
        hit.x = s1.x + ua * (s2.x - s1.x);
        hit.z = s1.z + ua * (s2.z - s1.z);
        //INFO << "INTERSECTING " << hit.x << " " << hit.z << std::endl;
        //INFO << ua << " | " << ub << std::endl;
        return ub;
      }
      //INFO << "NOT INTERSECTING" << std::endl;
      return -1.0f;
  }

  int Plane::segmentIntersect(const Vector3D & p1, const Vector3D & p2, Vector3D & p_p) {
    float d = - normal.x * pop.x - normal.y * pop.y - normal.z * pop.z;
    float denom = normal.x * (p2.x - p1.x) + normal.y * (p2.y - p1.y) +
      normal.z * (p2.z - p1.z);
    if (fabs(denom) < 0.001f)
      return false;
    float mu = - (d + normal.x * p1.x + normal.y * p1.y + normal.z * p1.z) / denom;
    p_p = Vector3D(p2 - p1) * mu + p1;
    if (mu < 0 || mu > 1)
      return false;
    return true;
  }
  float Plane::distance(const Vector3D & p) {
    float d = normal * p;
    d += - normal.x * pop.x - normal.y * pop.y - normal.z * pop.z;
    return d;
  }
}

/*
int main() {
  Vector3D p(0, 0, 0);
  Vector3D n(-1, 0, 0);
  Math::Plane plane(p, n);
  Vector3D p1(-1, 0, 0);
  Vector3D p2(1, 0, 0);
  Vector3D col;
  if (plane.segmentIntersect(p1, p2, col))
    std::cout << "intersect at: " << col.x << " " << col.y << " " << col.z << std::endl;
  std::cout << plane.distance(p1) << std::endl;
  std::cout << plane.distance(p2) << std::endl;
}
*/
