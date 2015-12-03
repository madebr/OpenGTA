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
#ifndef UTIL_IMAGE_LOADER_H
#define UTIL_IMAGE_LOADER_H

#include <SDL_opengl.h>
#include <string>
#include "gl_pagedtexture.h"

namespace ImageUtil {

  typedef std::pair<uint16_t, uint16_t> WidthHeightPair;

  struct NextPowerOfTwo {
    NextPowerOfTwo(uint32_t _w, uint32_t _h) {
      w = 1; h = 1;
      while (w < _w) { w <<= 1; }
      while (h < _h) { h <<= 1; }
    }
    uint32_t w;
    uint32_t h;
  };

  // hardcoded data for known images
  WidthHeightPair lookupImageSize(const std::string & name, const uint32_t size);
  // load a rgb image
  OpenGL::PagedTexture loadImageRAW(const std::string & name);
  // load a palette image and guess the palette filename
  OpenGL::PagedTexture loadImageRAT(const std::string & name);
  // load a palette image using palette file
  OpenGL::PagedTexture loadImageRATWithPalette(const std::string & name,
      const std::string & palette_file);
  // plain simple garden-variety create-a-texture; needs to be ^2
  GLuint createGLTexture(GLsizei w, GLsizei h, bool rgba, const void* pixels);

  // blitting a buffer into another; they should exist!
  void copyImage2Image(uint8_t *dest, const uint8_t *src, const uint16_t
    srcWidth, const uint16_t srcHeight, const uint16_t destWidth);

  OpenGL::PagedTexture createEmbeddedTexture(GLsizei w, GLsizei h, bool rgba, 
    const void *pixels);
}

#endif
