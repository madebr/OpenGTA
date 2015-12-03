#ifndef QUATERNION_MATH
#define QUATERNION_MATH
#include <cmath>

namespace Util { namespace Math {
  /*! 
   * \namespace Util::Math
   * \brief Vector/Matrix/Quaternion interpolation
   *
   * Found at http://www.flipcode.com/files/code/vquats.cpp
   *
   *
   * I think this is very sleek, so I kept it.
   *
   * Editor's note:  
   * COTD Entry:  Vector Math & Quaternions by Tim Sweeney [tim@epicgames.com]

       This is some fairly generic vector math code.  Here I want to
       illustrate two things:

       1. The "vectorSpace" template is a nice way to represent the
       general mathematical notion of vectors without regard to
       particular data types.  The idea is that you can declare
       a vector type (like vec4 below), and the vectorSpace template
       automatically generates the appropriate vector operators.

       I wrote this code for clarity and generality. For performance, you would 
       want to specialize the vectorSpace template for common types, like 4-
       component floating-point vectors. You could implement these using 
       KNI and 3DNow instructions, for example.

       2. Useful quaternion functions, most notably "exp" and "ln".

       The traditional and limited way programmers used quaternion
       rotations in the past was to use "spherical linear interpolation",
       interpolating a rotational arc between two quaternions at a constant
       angular rate.  Given quaternions q1 and q2, you might have used 
       a function lik "slerp(q1,q2,0.25)" to find a rotation
       one-quarter between the two quaternions.  This approach
       was unsatisfactory because it was difficult to perform
       smooth, higher order, i.e. hermite or bezier, interpolation
       between quaternions.

       New approach: take the logarithm ("ln" function below) of 
       your quaternions, then use ordinary linear interpolation 
       (sampleLinear below) or any higher-order interpolation
       scheme -- and then take the exponent ("exp") of the result.
       The resulting rotations will have the desired smooth angular
       movement rates.

       Why does this work?  Read up on "geometric algebra" to find out.
       Unit quaternions are just a special 3-dimensional case of 
       spinors, which are the most general representation of
       n-dimensional rotations possible.
       */

    // Template implementing a "vector space" (in the precise mathematical sense).
    // A vector space is a type "u", implemented as 
    // an array of "n" elements of type "t".
    template<class t,int n,class u> struct vectorSpace {
      t x[n];
      vectorSpace() {for(int i=0; i<n; i++) x[i]=t();}
      inline t& operator[](int i) {/*assert(i>=0 && i<n);*/ return x[i];}
      inline const t& operator[](int i) const {/*assert(i>=0 && i<n);*/ return x[i];}
      friend bool operator==    (const u&      a,const u&      b) {bool c=true;  for(int i=0; i<n; i++) c=c&&a[i]==b[i]; return c;}
      friend bool operator!=    (const u&      a,const u&      b) {bool c=false; for(int i=0; i<n; i++) c=c||a[i]!=b[i]; return c;}
      friend inline t vectorSum (const u&      a                ) {t    c=0;     for(int i=0; i<n; i++) c+=a[i];         return c;}
      friend inline t vectorProd(const u&      a                ) {t    c=1;     for(int i=0; i<n; i++) c*=a[i];         return c;}
      friend inline u operator- (const u&      a                ) {u c; for(int i=0; i<n; i++) c[i] =    -a[i]; return c;}
      friend inline u operator+ (const u&      a,const u&      b) {u c; for(int i=0; i<n; i++) c[i] =a[i]+b[i]; return c;}
      friend inline u operator- (const u&      a,const u&      b) {u c; for(int i=0; i<n; i++) c[i] =a[i]-b[i]; return c;}
      friend inline u operator* (const float   a,const u&      b) {u c; for(int i=0; i<n; i++) c[i] =a   *b[i]; return c;}
      friend inline u operator* (const u&      a,const float   b) {u c; for(int i=0; i<n; i++) c[i] =a[i]*b;    return c;}
      friend inline u operator/ (const u&      a,const float   b) {u c; for(int i=0; i<n; i++) c[i] =a[i]/b;    return c;}
      friend inline u operator+=(      u&      a,const u&      b) {     for(int i=0; i<n; i++) a[i]+=b[i];      return a;}
      friend inline u operator-=(      u&      a,const u&      b) {     for(int i=0; i<n; i++) a[i]-=b[i];      return a;}
      friend inline u operator*=(      u&      a,const float   b) {     for(int i=0; i<n; i++) a[i]*=b;         return a;}
      friend inline u operator/=(      u&      a,const float   b) {     for(int i=0; i<n; i++) a[i]/=b;         return a;}
    };
  template<class t> inline t sampleLinear(const t& a,const t& b,float s) {
    return (1-s)*a+s*b;
  }
  template<class t> inline t sampleHermite(const t& a,const t& b,const t& at,const t& bt,float s) {
    return
      (+2*s*s*s -3*s*s    +1)*a+
      (-2*s*s*s +3*s*s      )*b+
      (+1*s*s*s -2*s*s +s   )*at+
      (+1*s*s*s -1*s*s      )*bt;
  }
  template<class t> inline t sampleBezier(const t& a,const t& b,const t& ac,const t& bc,float s) {
    return
      (-1*s*s*s +3*s*s -3*s +1)*a+
      (+3*s*s*s -6*s*s +3*s   )*b+
      (-3*s*s*s +3*s*s        )*ac+
      (+1*s*s*s               )*bc;
  }

  // 4-component floating point vectors.
  struct vec4: public vectorSpace<float,4,vec4> {
    inline vec4() {x[0]=x[1]=x[2]=x[3]=0;}
    inline vec4(float _x,float _y,float _z,float _w) {x[0]=_x; x[1]=_y; x[2]=_z; x[3]=_w;}
  };

  // 4x4 matrices.
  struct mtx44: public vectorSpace<vec4,4,mtx44> {
    mtx44() {}
    mtx44(const vec4& _x,const vec4& _y, const vec4& _z,const vec4& _w) {x[0]=_x; x[1]=_y; x[2]=_z; x[3]=_w;}
  };

  // A 3-dimensional (4-component) quaternion.
  struct quat: public vectorSpace<float,4,quat> {
    quat() {x[0]=x[1]=x[2]=0; x[3]=0;}
    quat(float _x,float _y,float _z,float _w) {x[0]=_x; x[1]=_y; x[2]=_z; x[3]=_w;}
  };
  inline quat operator*(const quat& a,const quat& b) {
    return quat(
        +a[0]*b[3] +a[1]*b[2] -a[2]*b[1] +a[3]*b[0],
        -a[0]*b[2] +a[1]*b[3] +a[2]*b[0] +a[3]*b[1],
        +a[0]*b[1] -a[1]*b[0] +a[2]*b[3] +a[3]*b[2],
        -a[0]*b[0] -a[1]*b[1] -a[2]*b[2] +a[3]*b[3]
        );
  }
  inline float norm(const quat& a) {
    return a[0]*a[0] + a[1]*a[1] + a[2]*a[2] + a[3]*a[3];
  }
  inline float mag(const quat& a) {
    return sqrt(norm(a));
  }
  inline quat normal(const quat& a) {
    return a/mag(a);
  }
  inline quat conj(const quat& a) {
    return quat(-a[0],-a[1],-a[2],a[3]);
  }
  inline quat inv(const quat& a) {
    return 1.f/norm(a)*conj(a);
  }
  inline quat exp(const quat& a) {
    float r  = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    float et = std::exp(a[3]);
    float s  = r>=0.00001f? et*sin(r)/r: 0.f;
    return quat(s*a[0],s*a[1],s*a[2],et*cos(r));
  }
  inline quat ln(const quat& a) {
    float r  = sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
    float t  = r>0.00001f? atan2(r,a[3])/r: 0.f;
    return quat(t*a[0],t*a[1],t*a[2],0.5*log(norm(a)));
  }
  inline vec4 quatToRotationAxisAngle(const quat& a) {
    float t   = a[3]*a[3];
    float rsa = t<0.99999999? 1.f/sqrt(1.f-t): 1.f;
    return vec4(a[0]*rsa,a[1]*rsa,a[2]*rsa,acos(a[3])*2.f);
  }
  inline quat quatFromRotationAxisAngle(const vec4& a) {
    float s = sin(a[3]/2.f);
    float c = cos(a[3]/2.f);
    return quat(a[0]*s,a[1]*s,a[2]*s,c);
  }
  inline mtx44 quatToRotationMatrix(const quat& a) {
    float xx=a[0]*a[0];
    float xy=a[0]*a[1], yy=a[1]*a[1];
    float xz=a[0]*a[2], yz=a[1]*a[2], zz=a[2]*a[2];
    float xw=a[0]*a[3], yw=a[1]*a[3], zw=a[2]*a[3], ww=a[3]*a[3];
    return mtx44(
        vec4(+xx-yy-zz+ww, +xy+zw+xy+zw, +xz-yw+xz-yw, 0),
        vec4(+xy-zw+xy-zw, -xx+yy-zz+ww, +yz+xw+yz+xw, 0),
        vec4(+xz+yw+xz+yw, +yz-xw+yz-xw, -xx-yy+zz+ww, 0),
        vec4(0           , 0           , 0           , 1)
        );
  }
  inline quat quatFromRotationMatrix(const mtx44& m) {
    float w=0.5*sqrt(m[0][0]+m[1][1]+m[2][2]+m[3][3]);
    float s=0.25/w;
    return quat(
        (m[1][2]-m[2][1])*s,
        (m[2][0]-m[0][2])*s,
        (m[0][1]-m[1][0])*s,
        w
        );
  }
}
}

#endif
