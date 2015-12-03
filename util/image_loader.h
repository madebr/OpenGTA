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
