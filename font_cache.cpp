#include "font_cache.h"

namespace OpenGTA {
  FontCache::FontCache() {
  }

  FontCache::~FontCache() {
  }

  OpenGL::DrawableFont & FontCache::getFont(const std::string & file,
    const uint32_t scale) {
    FontMap::iterator i = findFont(file, scale);
    if (i == loadedFonts.end()) {
      OpenGL::DrawableFont* fnt = createFont(file, scale);
      assert(fnt);
      loadedFonts.insert(std::make_pair<FontIdentifier, OpenGL::DrawableFont*>(
        FontIdentifier(file, scale), fnt));
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
