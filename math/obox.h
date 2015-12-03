#include "math3d.h"

// see: from: http://www.3dkingdoms.com/weekly/weekly.php?a=21

// basically the same as bbox.h/.cpp but using coldet math 

class OBox
{
  public:
    OBox() {}
    OBox( const Matrix3D & m, const Vector3D & extent ) 
    { Set( m, extent );	}
    OBox( const Matrix3D & m, const Vector3D & low, const Vector3D & high ) 
    { Set( m, low, high );	}
    OBox( const OBox & other)
    { Set( other.m_M, other.m_Extent ); }

    void Set( const Matrix3D & m, const Vector3D & extent )
    {
      m_M = m;
      m_Extent = extent;
    }	
    void Set( const Matrix3D & m, const Vector3D & low, const Vector3D & high )
    {
      m_M = m;
      m_M.Translate( 0.5f * (low + high) );
      m_Extent = 0.5f * (high - low);
    }

    Vector3D GetSize() const 
    { return 2.0f * m_Extent; }
    Vector3D GetCenterPoint() const 
    { return m_M.GetTranslate(); }		
    void GetInvRot( Vector3D *pvRot ) const;

    bool IsPointInBox( const Vector3D & p ) const ;
    bool IsBoxInBox( OBox & box ) const ;
    bool IsSphereInBox( const Vector3D & p, float fRadius ) const ;
    bool IsLineInBox( const Vector3D & l1, const Vector3D & l2 ) const ;
    bool BoxOutsidePlane( const Vector3D & normal, const Vector3D & p ) const ;

    // Data
    Matrix3D m_M;
    Vector3D m_Extent;		
};
