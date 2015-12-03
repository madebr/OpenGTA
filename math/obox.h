/* Derived from code written by Jonathan Kreuzer.
 *
 * See: http://www.3dkingdoms.com/weekly/weekly.php?a=21
 *
 * basically the same as bbox.h/.cpp but using coldet math 
 *
 * -- quote from a mail of the author --
 *
 * You're free to continue using my CBBox code however you want. 
 * ... [snip] ... 
 * The only thing I ask is a note about where it came from ( I think
 * you said you added a link to the article, so that's fine. )
 *
 */

/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/

#include "math3d.h"

class OBox
{
  public:
    OBox() {}
    OBox( const Matrix3D & m, const Vector3D & extent ) 
    { set( m, extent );	}
    OBox( const Matrix3D & m, const Vector3D & low, const Vector3D & high ) 
    { set( m, low, high );	}
    OBox( const OBox & other)
    { set( other.m_M, other.m_Extent ); }

    void set( const Matrix3D & m, const Vector3D & extent )
    {
      m_M = m;
      m_Extent = extent;
    }	
    void set( const Matrix3D & m, const Vector3D & low, const Vector3D & high )
    {
      m_M = m;
      m_M.Translate( 0.5f * (low + high) );
      m_Extent = 0.5f * (high - low);
    }

    Vector3D getSize() const 
    { return 2.0f * m_Extent; }
    Vector3D getCenterPoint() const 
    { return m_M.GetTranslate(); }		
    void getInvRot( Vector3D *pvRot ) const;

    bool isPointInBox( const Vector3D & p ) const ;
    bool isBoxInBox( OBox & box ) const ;
    bool isSphereInBox( const Vector3D & p, float fRadius ) const ;
    bool isLineInBox( const Vector3D & l1, const Vector3D & l2 ) const ;
    bool boxOutsidePlane( const Vector3D & normal, const Vector3D & p ) const ;

    void lineCrossBox(const Vector3D & l1, const Vector3D & l2, Vector3D & isecLocalSpace) const;

    // Data
    Matrix3D m_M;
    Vector3D m_Extent;		
};
