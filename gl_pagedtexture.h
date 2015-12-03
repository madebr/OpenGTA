/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
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
#ifndef PAGED_TEXTURE_H
#define PAGED_TEXTURE_H

#include <SDL_opengl.h>

namespace OpenGL {

  struct TexCoord {
    TexCoord(GLfloat a, GLfloat b) : u(a), v(b) {}
    TexCoord() : u(0.0f), v(0.0f) {}
    GLfloat u;
    GLfloat v;
  };

  struct PagedTexture {
    private:
      void _copyCoords(const PagedTexture & other) {
        for (int i=0; i < 2; ++i) {
          coords[i].u = other.coords[i].u;
          coords[i].v = other.coords[i].v;
        }

      }
    public:
      PagedTexture(GLuint p, GLfloat a, GLfloat b, GLfloat c, GLfloat d) :
        inPage(p) {
          coords[0].u = a;
          coords[0].v = b;
          coords[1].u = c;
          coords[1].v = d;
        }
      PagedTexture() : inPage(0) {}
      PagedTexture(const PagedTexture & other) : inPage(other.inPage) {
        _copyCoords(other);
      }
      PagedTexture & operator = (const PagedTexture & other) {
        inPage = other.inPage;
        _copyCoords(other);
        return *this;
      }
      GLuint inPage;
      TexCoord coords[2];
  };

}
#endif
