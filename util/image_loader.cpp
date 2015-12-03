#include <cstddef>
#include <physfs.h>
#include "image_loader.h"
#include "file_helper.h"
#include "buffercache.h"
#include "log.h"
#include "cistring.h"
#include "opengta.h"

namespace ImageUtil {
using OpenGL::PagedTexture;

  WidthHeightPair lookupImageSize(const std::string & name, const uint32_t size) {
    Util::ci_string iname(name.c_str());
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
    return std::make_pair<uint16_t, uint16_t>(width, height);
  }


  OpenGL::PagedTexture loadImageRAW(const std::string & name) {

    PHYSFS_file * fd = GET_FILE_HELPER.openReadVFS(name);

    uint32_t nbytes = PHYSFS_fileLength(fd);

    WidthHeightPair whp = lookupImageSize(name, nbytes);

    if (whp.first == 0 || whp.second == 0) {
      PHYSFS_close(fd);
      WARN << "aborting image load" << std::endl;
    }

    Util::BufferCache::LockedBuffer lbuf(nbytes);
    /*
    uint8_t * buffer = Util::BufferCacheHolder::Instance().
      requestBuffer(nbytes);
    */
    uint8_t *buffer = lbuf();
    PHYSFS_read(fd, buffer, 1, nbytes);
    PHYSFS_close(fd);

    return createEmbeddedTexture(whp.first, whp.second, false, buffer); 
  }

  
  OpenGL::PagedTexture loadImageRATWithPalette(const std::string & name,
      const std::string & palette_file) {
    
    Util::FileHelper & fh = GET_FILE_HELPER;

    PHYSFS_file * fd = fh.openReadVFS(name);
    uint32_t nbytes = PHYSFS_fileLength(fd);

    WidthHeightPair whp = lookupImageSize(name, nbytes);
    if (whp.first == 0 || whp.second == 0) {
      PHYSFS_close(fd);
      WARN << "aborting image load" << std::endl;
    }
    Util::BufferCache::LockedBuffer lb1(nbytes);
    PHYSFS_read(fd, lb1(), 1, nbytes);
    PHYSFS_close(fd);

    // if this causes an exception, the buffercache will cleanup
    fd = fh.openReadVFS(palette_file);
    OpenGTA::Graphics8Bit::RGBPalette rgb(fd);
    PHYSFS_close(fd);

    Util::BufferCache::LockedBuffer lb2(nbytes * 3);
    rgb.apply(nbytes, lb1(), lb2(), false);

    return createEmbeddedTexture(whp.first, whp.second, false, lb2());
  }

  GLuint createGLTexture(GLsizei w, GLsizei h, bool rgba, const void* pixels) {
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if (rgba)
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    else
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixels);

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
    uint32_t bpp = (rgba ? 4 : 3);

    uint32_t bufSize = npot.w * npot.h * bpp;
    uint8_t* buff = Util::BufferCacheHolder::Instance().requestBuffer(bufSize);
    copyImage2Image(buff, (uint8_t*)pixels, w * bpp, h, npot.w * bpp);

    GLuint tex = createGLTexture(npot.w, npot.h, rgba, buff);
    return PagedTexture(tex, 0, 0, float(w) / npot.w, float(h) / npot.h);
  }
}
