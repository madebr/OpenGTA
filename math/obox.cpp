#include "obox.h"
// --------------------------
//
// Oriented Bounding Box Class
//
// --------------------------

//
// Check if a point is in this bounding box
//
bool OBox::IsPointInBox(const Vector3D &InP)
{
  // Rotate the point into the box's coordinates
  Vector3D P = Transform(InP, m_M.Inverse());

  // Now just use an axis-aligned check
  if ( fabs(P.x) < m_Extent.x && fabs(P.y) < m_Extent.y && fabs(P.z) < m_Extent.z ) 
    return true;

  return false;
}

//
// Check if a sphere overlaps any part of this bounding box
//
bool OBox::IsSphereInBox( const Vector3D &InP, float fRadius)
{
  float fDist;
  float fDistSq = 0;
  Vector3D P = Transform(InP, m_M.Inverse()); 

  // Add distance squared from sphere centerpoint to box for each axis
  for ( int i = 0; i < 3; i++ )
  {
    if ( fabs(P[i]) > m_Extent[i] )
    {
      fDist = fabs(P[i]) - m_Extent[i];
      fDistSq += fDist*fDist;
    }
  }
  return ( fDistSq <= fRadius*fRadius );
}

//
// Check if the bounding box is completely behind a plane( defined by a normal and a point )
//
bool OBox::BoxOutsidePlane( const Vector3D &InNorm, const Vector3D &InP )
{
  // Plane Normal in Box Space
  Vector3D Norm = rotateVector(InNorm, m_M.Inverse() );
  Norm = Vector3D( fabs( Norm.x ), fabs( Norm.y ), fabs( Norm.z ) );

  float Extent = Norm * m_Extent; //Norm.Dot( m_Extent ); // Box Extent along the plane normal
  //float Distance = InNorm.Dot( GetCenterPoint() - InP ); // Distance from Box Center to the Plane
  float Distance = InNorm * (GetCenterPoint() - InP);

  // If Box Centerpoint is behind the plane further than its extent, the Box is outside the plane
  if ( Distance < -Extent ) return true;
  return false;
}

//
// Does the Line (L1, L2) intersect the Box?
//
bool OBox::IsLineInBox( const Vector3D& L1, const Vector3D& L2 )
{	
  // Put line in box space
  Matrix3D MInv = m_M.Inverse();
  Vector3D LB1 = Transform(L1, MInv);
  Vector3D LB2 = Transform(L2, MInv);

  // Get line midpoint and extent
  Vector3D LMid = (LB1 + LB2) * 0.5f; 
  Vector3D L = (LB1 - LMid);
  Vector3D LExt = Vector3D( fabs(L.x), fabs(L.y), fabs(L.z) );

  // Use Separating Axis Test
  // Separation vector from box center to line center is LMid, since the line is in box space
  if ( fabs( LMid.x ) > m_Extent.x + LExt.x ) return false;
  if ( fabs( LMid.y ) > m_Extent.y + LExt.y ) return false;
  if ( fabs( LMid.z ) > m_Extent.z + LExt.z ) return false;
  // Crossproducts of line and each axis
  if ( fabs( LMid.y * L.z - LMid.z * L.y)  >  (m_Extent.y * LExt.z + m_Extent.z * LExt.y) ) return false;
  if ( fabs( LMid.x * L.z - LMid.z * L.x)  >  (m_Extent.x * LExt.z + m_Extent.z * LExt.x) ) return false;
  if ( fabs( LMid.x * L.y - LMid.y * L.x)  >  (m_Extent.x * LExt.y + m_Extent.y * LExt.x) ) return false;
  // No separating axis, the line intersects
  return true;
}

//
// Returns a 3x3 rotation matrix as vectors
//
inline void OBox::GetInvRot( Vector3D *pvRot )
{
  pvRot[0] = Vector3D( m_M.m[0][0], m_M.m[0][1], m_M.m[0][2] );
  pvRot[1] = Vector3D( m_M.m[1][0], m_M.m[1][1], m_M.m[1][2] );
  pvRot[2] = Vector3D( m_M.m[2][0], m_M.m[2][1], m_M.m[2][2] );
}

//
// Check if any part of a box is inside any part of another box
// Uses the separating axis test.
//
bool OBox::IsBoxInBox( OBox &BBox )
{
  Vector3D SizeA = m_Extent;
  Vector3D SizeB = BBox.m_Extent;
  Vector3D RotA[3], RotB[3];	
  GetInvRot( RotA );
  BBox.GetInvRot( RotB );

  float R[3][3];  // Rotation from B to A
  float AR[3][3]; // absolute values of R matrix, to use with box extents
  float ExtentA, ExtentB, Separation;
  int i, k;

  // Calculate B to A rotation matrix
  for( i = 0; i < 3; i++ )
    for( k = 0; k < 3; k++ )
    {
      R[i][k] = RotA[i] * RotB[k]; 
      AR[i][k] = fabs(R[i][k]);
    }

  // Vector separating the centers of Box B and of Box A	
  Vector3D vSepWS = BBox.GetCenterPoint() - GetCenterPoint();
  // Rotated into Box A's coordinates
  Vector3D vSepA( vSepWS * RotA[0], vSepWS * RotA[1], vSepWS * RotA[2] );            

  // Test if any of A's basis vectors separate the box
  for( i = 0; i < 3; i++ )
  {
    ExtentA = SizeA[i];
    ExtentB = SizeB * Vector3D( AR[i][0], AR[i][1], AR[i][2] );
    Separation = fabs( vSepA[i] );

    if( Separation > ExtentA + ExtentB ) return false;
  }

  // Test if any of B's basis vectors separate the box
  for( k = 0; k < 3; k++ )
  {
    ExtentA = SizeA * Vector3D( AR[0][k], AR[1][k], AR[2][k] );
    ExtentB = SizeB[k];
    Separation = fabs( vSepA * Vector3D(R[0][k],R[1][k],R[2][k]) );

    if( Separation > ExtentA + ExtentB ) return false;
  }

  // Now test Cross Products of each basis vector combination ( A[i], B[k] )
  for( i=0 ; i<3 ; i++ )
    for( k=0 ; k<3 ; k++ )
    {
      int i1 = (i+1)%3, i2 = (i+2)%3;
      int k1 = (k+1)%3, k2 = (k+2)%3;
      ExtentA = SizeA[i1] * AR[i2][k]  +  SizeA[i2] * AR[i1][k];
      ExtentB = SizeB[k1] * AR[i][k2]  +  SizeB[k2] * AR[i][k1];
      Separation = fabs( vSepA[i2] * R[i1][k]  -  vSepA[i1] * R[i2][k] );
      if( Separation > ExtentA + ExtentB ) return false;
    }

  // No separating axis found, the boxes overlap	
  return true;
}
