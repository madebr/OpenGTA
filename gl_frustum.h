//***********************************************************************//
//                                                                       //
//           - "Talk to me like I'm a 3 year old!" Programming Lessons - //
//                                                                       //
//           $Author: DigiBen        digiben@gametutorials.com           //
//                                                                       //
//           $Program:               Frustum Culling                     //
//                                                                       //
//           $Description:   Demonstrates checking if shapes are in view //
//                                                                       //
//           $Date:                  8/28/01                             //
//                                                                       //
//***********************************************************************//

#ifndef DIGIBEN_FRUSTUM
#define DIGIBEN_FRUSTUM

namespace Util {

  // This will allow us to create an object to keep track of our frustum
  class CFrustum {

    public:

      // Call this every time the camera moves to update the frustum
      void CalculateFrustum();

      // This takes a 3D point and returns TRUE if it's inside of the frustum
      bool PointInFrustum(float x, float y, float z);

      // This takes a 3D point and a radius and returns TRUE if the sphere is inside of the frustum
      bool SphereInFrustum(float x, float y, float z, float radius);

      // This takes the center and half the length of the cube.
      bool CubeInFrustum( float x, float y, float z, float size );

      bool BlockInFrustum(float x, float z, float size);

    private:

      // This holds the A B C and D values for each side of our frustum.
      float m_Frustum[6][4];
  };

}

#endif
