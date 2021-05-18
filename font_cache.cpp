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
#include "font_cache.h"

#include <cassert>

namespace OpenGTA {
  FontCache::~FontCache() {
    for (FontMap::iterator i = loadedFonts.begin(); i != loadedFonts.end(); i++) {
      delete i->second;
    }
    loadedFonts.clear();
  }

  OpenGL::DrawableFont & FontCache::getFont(const std::string & file,
    const uint32_t scale) {
    FontMap::iterator i = findFont(file, scale);
    if (i == loadedFonts.end()) {
      OpenGL::DrawableFont* fnt = createFont(file, scale);
      assert(fnt);
      loadedFonts.insert(std::make_pair(FontIdentifier(file, scale), fnt));
      return *fnt;
    }
    return *i->second;
  }

  FontCache::FontMap::iterator FontCache::findFont(const std::string & file, const uint32_t & scale) {
    return loadedFonts.find(FontIdentifier(file, scale));
  }

  OpenGL::DrawableFont* FontCache::createFont(const std::string & file, const uint32_t & scale) {
    OpenGL::DrawableFont * fnt = new OpenGL::DrawableFont();
    fnt->setScale(scale);
    fnt->loadFont(file);
    return fnt;
  }
}
