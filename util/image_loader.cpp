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
#include <cstddef>
#include <physfs.h>
#include "image_loader.h"
#include "file_helper.h"
#include "buffercache.h"
#include "log.h"
#include "opengta.h"
#include "physfsrwops.h"
#include "SDL_image.h"
#include "m_exceptions.h"
#include "string_helpers.h"

namespace ImageUtil {
using OpenGL::PagedTexture;

  WidthHeightPair lookupImageSize(const std::string & name, const uint32_t size) {
    std::string iname { Util::string_upper(name) };
    uint16_t width = 0;
    uint16_t height = 0;
    uint32_t bpp = 0;

    // m4 tools/raw_images.m4
    if ((iname.find("CUT") == 0) && (iname.find(".RA") == 4)) {
      width = 640; height = 480;
    }
    if ((iname.find("F_BMG.RA") == 0)) {
      width = 100; height = 50;
    }
    if ((iname.find("F_DMA.RA") == 0)) {
      width = 78; height = 109;
    }
    if ((iname.find("F_LOGO") == 0) && (iname.find(".RA") == 7)) {
      width = 640; height = 168;
    }
    if ((iname.find("F_LOWER") == 0) && (iname.find(".RA") == 8)) {
      width = 640; height = 312;
    }
    if ((iname.find("F_PLAYN.RA") == 0)) {
      width = 180; height = 50;
    }
    if ((iname.find("F_PLAY") == 0) && (iname.find(".RA") == 7)) {
      width = 102; height = 141;
    }
    if ((iname.find("F_UPPER.RA") == 0)) {
      width = 640; height = 168;
    }

    // end-of-generated code
    if (iname.find(".RAW") == iname.length() - 4)
      bpp = 3;

    if (iname.find(".RAT") == iname.length() - 4)
      bpp = 1;

    if (!(bpp && bpp * width * height == size))
      ERROR << "could not identify image: " << name << " size: " << size << std::endl;
    return std::make_pair(width, height);
  }


  OpenGL::PagedTexture loadImageRAW(const std::string & name) {

    PHYSFS_file * fd = Util::FileHelper::OpenReadVFS(name);

    uint32_t nbytes = PHYSFS_fileLength(fd);

    WidthHeightPair whp = lookupImageSize(name, nbytes);

    if (whp.first == 0 || whp.second == 0) {
      PHYSFS_close(fd);
      WARN << "aborting image load" << std::endl;
      throw E_UNKNOWNKEY(name + " - RAW file size unknown");
    }

    Util::BufferCache::LockedBuffer lbuf(nbytes);
    /*
    uint8_t * buffer = Util::BufferCache::Instance().
      requestBuffer(nbytes);
    */
    uint8_t *buffer = lbuf();
    PHYSFS_readBytes(fd, buffer, nbytes);
    PHYSFS_close(fd);

    return createEmbeddedTexture(whp.first, whp.second, false, buffer);
  }


  OpenGL::PagedTexture loadImageRATWithPalette(const std::string & name,
      const std::string & palette_file) {

    PHYSFS_file * fd = Util::FileHelper::OpenReadVFS(name);
    uint32_t nbytes = PHYSFS_fileLength(fd);

    WidthHeightPair whp = lookupImageSize(name, nbytes);
    if (whp.first == 0 || whp.second == 0) {
      PHYSFS_close(fd);
      WARN << "aborting image load" << std::endl;
      throw E_UNKNOWNKEY(name + " - RAT file size unknown");
    }
    Util::BufferCache::LockedBuffer lb1(nbytes);
    PHYSFS_readBytes(fd, lb1(), nbytes);
    PHYSFS_close(fd);

    // if this causes an exception, the buffercache will cleanup
    fd = Util::FileHelper::OpenReadVFS(palette_file);
    OpenGTA::Graphics8Bit::RGBPalette rgb(fd);
    PHYSFS_close(fd);

    Util::BufferCache::LockedBuffer lb2(nbytes * 3);
    rgb.apply(nbytes, lb1(), lb2(), false);

    return createEmbeddedTexture(whp.first, whp.second, false, lb2());
  }

#ifdef WITH_SDL_IMAGE
  OpenGL::PagedTexture loadImageSDL(const std::string & name) {
    SDL_RWops * rwops = PHYSFSRWOPS_openRead(name.c_str());
    SDL_Surface *surface = IMG_Load_RW(rwops, 1);
    assert(surface);

    NextPowerOfTwo npot(surface->w, surface->h);
    uint16_t bpp = surface->format->BytesPerPixel;

    uint8_t * buffer = Util::BufferCache::Instance().requestBuffer(npot.w * npot.h * bpp);
    SDL_LockSurface(surface);
    copyImage2Image(buffer, (uint8_t*)surface->pixels, surface->pitch, surface->h,
        npot.w * bpp);
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(surface);

    GLuint texture = createGLTexture(npot.w, npot.h, (bpp == 4) ? true : false, buffer);
    return OpenGL::PagedTexture(texture, 0, 0, GLfloat(surface->w)/npot.w,
      GLfloat(surface->h)/npot.h);
  }
#endif

  GLuint createGLTexture(GLsizei w, GLsizei h, bool rgba, const void* pixels) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (!mipmapTextures)
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    else
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (rgba) {
      if (mipmapTextures)
        gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
      else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    }
    else {
      if (mipmapTextures)
        gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
      else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    }
    if (supportedMaxAnisoDegree > 1.0f)
      glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, &supportedMaxAnisoDegree);

    GL_CHECKERROR;
    return tex;
  }

  void copyImage2Image(uint8_t *dest, const uint8_t *src, const uint16_t
    srcWidth, const uint16_t srcHeight, const uint16_t destWidth) {
    uint8_t *d = dest;
    uint32_t srcOff = 0;
    for (uint16_t j = 0; j < srcHeight; ++j) {
      memcpy(d, src + srcOff, srcWidth);
      srcOff += srcWidth;
      d += destWidth;
    }
  }

  OpenGL::PagedTexture createEmbeddedTexture(GLsizei w, GLsizei h,
      bool rgba, const void* pixels) {

    NextPowerOfTwo npot(w, h);
    uint8_t* buff = (uint8_t*)pixels;

    if (!(npot.w == uint32_t(w) && npot.h == uint32_t(h))) {
      uint32_t bpp = (rgba ? 4 : 3);
      uint32_t bufSize = npot.w * npot.h * bpp;
      buff = Util::BufferCache::Instance().requestBuffer(bufSize);
      copyImage2Image(buff, (uint8_t*)pixels, w * bpp, h, npot.w * bpp);
    }

    GLuint tex = createGLTexture(npot.w, npot.h, rgba, buff);
    return PagedTexture(tex, 0, 0, float(w) / npot.w, float(h) / npot.h);
  }

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))
#define READINT24(x)      ((x)[0]<<16 | (x)[1]<<8 | (x)[2])
#define WRITEINT24(x, i)  {(x)[0]=i>>16; (x)[1]=(i>>8)&0xff; x[2]=i&0xff; }

  uint8_t* scale2x_24bit(const uint8_t* src, const int src_width, const int src_height) {
    const int srcpitch = src_width * 3;
    const int dstpitch = src_width * 6;

    uint8_t *dstpix = Util::BufferCache::Instance().requestBuffer(src_width *
        src_height * 3 * 4);
    int E0, E1, E2, E3, B, D, E, F, H;
    for(int looph = 0; looph < src_height; ++looph)
    {
      for(int loopw = 0; loopw < src_width; ++ loopw)
      {
        B = READINT24(src + (MAX(0,looph-1)*srcpitch) + (3*loopw));
        D = READINT24(src + (looph*srcpitch) + (3*MAX(0,loopw-1)));
        E = READINT24(src + (looph*srcpitch) + (3*loopw));
        F = READINT24(src + (looph*srcpitch) + (3*MIN(src_width-1,loopw+1)));
        H = READINT24(src + (MIN(src_height-1,looph+1)*srcpitch) + (3*loopw));

        E0 = D == B && B != F && D != H ? D : E;
        E1 = B == F && B != D && F != H ? F : E;
        E2 = D == H && D != B && H != F ? D : E;
        E3 = H == F && D != H && B != F ? F : E;

        WRITEINT24((dstpix + looph*2*dstpitch + loopw*2*3), E0);
        WRITEINT24((dstpix + looph*2*dstpitch + (loopw*2+1)*3), E1);
        WRITEINT24((dstpix + (looph*2+1)*dstpitch + loopw*2*3), E2);
        WRITEINT24((dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*3), E3);
      }
    }
    return dstpix;
  }

  uint8_t* scale2x_32bit(const uint8_t* src, const int src_width, const int src_height) {
    const int srcpitch = src_width * 4;
    const int dstpitch = src_width * 8;

    uint8_t* dstpix = Util::BufferCache::Instance().requestBuffer(src_width *
        src_height * 4 * 4);
    Uint32 E0, E1, E2, E3, B, D, E, F, H;
    for(int looph = 0; looph < src_height; ++looph)
    {
      for(int loopw = 0; loopw < src_width; ++ loopw)
      {
        B = *(Uint32*)(src + (MAX(0,looph-1)*srcpitch) + (4*loopw));
        D = *(Uint32*)(src + (looph*srcpitch) + (4*MAX(0,loopw-1)));
        E = *(Uint32*)(src + (looph*srcpitch) + (4*loopw));
        F = *(Uint32*)(src + (looph*srcpitch) + (4*MIN(src_width-1,loopw+1)));
        H = *(Uint32*)(src + (MIN(src_height-1,looph+1)*srcpitch) + (4*loopw));

        E0 = D == B && B != F && D != H ? D : E;
        E1 = B == F && B != D && F != H ? F : E;
        E2 = D == H && D != B && H != F ? D : E;
        E3 = H == F && D != H && B != F ? F : E;

        *(Uint32*)(dstpix + looph*2*dstpitch + loopw*2*4) = E0;
        *(Uint32*)(dstpix + looph*2*dstpitch + (loopw*2+1)*4) = E1;
        *(Uint32*)(dstpix + (looph*2+1)*dstpitch + loopw*2*4) = E2;
        *(Uint32*)(dstpix + (looph*2+1)*dstpitch + (loopw*2+1)*4) = E3;
      }
    }
    return dstpix;
  }

  bool    mipmapTextures          = false;
  GLfloat supportedMaxAnisoDegree = 1.0f;
}
