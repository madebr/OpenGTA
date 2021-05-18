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
#ifndef SPRITE_CACHE_H
#define SPRITE_CACHE_H
#include <cstddef>
#include <physfs.h>
#include "gl_pagedtexture.h"
#include "gl_texturecache.h"
#include "opengta.h"

namespace OpenGL {
  
  struct SpriteIdentifier;
  struct SpriteIdentifier {
    PHYSFS_uint16 sprNum;
    PHYSFS_sint16 remap;
    PHYSFS_uint32 delta;
    SpriteIdentifier();
    SpriteIdentifier(PHYSFS_uint16, PHYSFS_sint16, PHYSFS_uint32);
    SpriteIdentifier(const SpriteIdentifier & other);
    bool operator ==(const SpriteIdentifier & other) const;
    bool operator <(const SpriteIdentifier & other) const;
  };
  
  class SpriteCache {
    private:
      SpriteCache();
      ~SpriteCache();
    public:
      SpriteCache(const SpriteCache& copy) = delete;
      SpriteCache& operator=(const SpriteCache& copy) = delete;

      static SpriteCache& Instance()
      {
        static SpriteCache instance;
        return instance;
      }

      void           clearAll();
      bool           getScale2x();
      void           setScale2x(bool enabled);
      bool           has(PHYSFS_uint16 sprNum);
      bool           has(PHYSFS_uint16 sprNum, PHYSFS_sint16 remap);
      bool           has(const SpriteIdentifier & si);
      PagedTexture & get(PHYSFS_uint16 sprNum);
      PagedTexture & get(PHYSFS_uint16 sprNum, PHYSFS_sint16 remap);
      PagedTexture & get(const SpriteIdentifier & si);
      void           add(PHYSFS_uint16 sprNum, PagedTexture & t);
      void           add(PHYSFS_uint16 sprNum, PHYSFS_sint16 remap, PagedTexture & t);
      void           add(const SpriteIdentifier & si, PagedTexture & t);
      PagedTexture   create(PHYSFS_uint16 sprNum, 
        OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes, PHYSFS_sint16 remap);
      PagedTexture   create(PHYSFS_uint16 sprNum,
        OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes,
        PHYSFS_sint16 remap, PHYSFS_uint32 delta);

      OpenGL::PagedTexture createSprite(size_t sprite_num, PHYSFS_sint16 remap, 
        PHYSFS_uint32 delta, OpenGTA::GraphicsBase::SpriteInfo* info);
    private:

      typedef std::map<SpriteIdentifier, PagedTexture> SpriteMapType;
      SpriteMapType  loadedSprites;
      bool           doScale2x;
  };
}

#endif
