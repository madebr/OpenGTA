#ifndef FONT_CACHE_H
#define FONT_CACHE_H
#include <string>
#include <map>
#include "gl_font.h"
#include "Singleton.h"

namespace OpenGTA {
  
  class FontCache {
    public:
      FontCache();
      ~FontCache();
      OpenGL::DrawableFont & getFont(const std::string & file, const uint32_t scale);

    private:
      struct FontIdentifier {
        FontIdentifier(const std::string & f, const uint32_t s) : filename(f), scale(s) {}
        const std::string filename;
        const uint32_t scale;
        bool operator == (const FontIdentifier & o) const {
          if (scale == o.scale && filename == o.filename)
            return true;
          return false;
        }
        bool operator < (const FontIdentifier & o) const {
          if (scale < o.scale)
            return true;
          if (scale > o.scale)
            return false;
          return filename < o.filename;
        }
      };
      typedef std::map<FontIdentifier, OpenGL::DrawableFont*> FontMap;
      FontMap loadedFonts;
      FontMap::iterator findFont(const std::string & file, const uint32_t & scale);
      OpenGL::DrawableFont* createFont(const std::string & file, const uint32_t & scale);
  };

  typedef Loki::SingletonHolder<FontCache, Loki::CreateUsingNew, 
    Loki::DefaultLifetime, Loki::SingleThreaded> FontCacheHolder;
}

#endif
