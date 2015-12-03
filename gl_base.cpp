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
#include "gl_base.h"
#include <SDL_opengl.h>
#include "common_sdl_gl.h"
#include "log.h"

using namespace Util;

namespace OpenGL {
  template<class T> void ImmediateRenderer<T>::assertCorrectPrimitive(GLenum newType) {
    if (!insideBegin) {
      WARN << "Missing glBegin() call" << std::endl;
    }
    else {
      if (currentPrimitiveType == newType)
        return;
      WARN << "Forcing switch of primitive type" << std::endl;
      glEnd();
    }
    glBegin(newType);
    currentPrimitiveType = newType;
    insideBegin = true;
  }

  template<class T> void ImmediateRenderer<T>::begin(GLenum type) {
    // FIXME: check !insideBegin
    currentPrimitiveType = type;
    insideBegin = true;
  }
  template<class T> void ImmediateRenderer<T>::end() {
    insideBegin = false;
  }
   
  template<class T> GLenum ImmediateRenderer<T>::currentPrimitiveType = GL_POINTS;

  template<> void ImmediateRenderer<Quad2Int>::draw(const Quad2Int & quad) {
    //assertCorrectPrimitive(quad.primitiveType);
    for (int i = 0; i < 4; i++)
      glVertex2i(quad.vertices[i][0], quad.vertices[i][1]);
  }
  template<> void ImmediateRenderer<ColoredQuad2Int>::draw(const ColoredQuad2Int & quad) {
    //assertCorrectPrimitive(quad.primitiveType);
    for (int i = 0; i < 4; i++) {
      glColor3f(quad.colors[i][0], quad.colors[i][1], quad.colors[i][2]);
      glVertex2i(quad.vertices[i][0], quad.vertices[i][1]);
    }
  }
  template<> void ImmediateRenderer<FontQuad>::draw(const FontQuad & quad) {
    glBindTexture(GL_TEXTURE_2D, quad.texId);
    glBegin(quad.primitiveType);
    for (int i = 0; i < 4; i++) {
      glTexCoord2f(quad.texCoords[i][0], quad.texCoords[i][1]);
      glVertex2i(quad.vertices[i][0], quad.vertices[i][1]);
    }
    glEnd();
  }

  /*
  template<> void ImmediateRenderer<OpenGTA::Map::BlockInfo>::draw(const OpenGTA::Map::BlockInfo & info) {
    for (int i = 0; i < 4; ++i) {
    }
  }
  */
}
