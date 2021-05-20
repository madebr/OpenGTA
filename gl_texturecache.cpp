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
#include <iostream>
#include <limits>
#include "gl_texturecache.h"
#include "log.h"

namespace OpenGL {
  template <typename key_type>
    TextureCache<key_type>::TextureCache(const char* with_name) : m_name(with_name) {
      instance_id = instance_count++;
      clearMagic = 0;
      has_cached_query = false;
      last_query_result = 0;
      minClearElements = 50;
    }
  template <typename key_type>
    TextureCache<key_type>::TextureCache() {
      instance_id = instance_count++;
      m_name = "TextureCache_" + std::to_string(instance_count);
      has_cached_query = false;
      clearMagic = 0;
      minClearElements = 50;
    }

  template <typename key_type>
    TextureCache<key_type>::~TextureCache() {
      unsigned int ts = cached.size();
      clearAll();
      INFO << m_name << " exited - " << ts << " textures recycled" << std::endl;
      m_name.clear();
      instance_count--;
    }

  template <typename key_type>
    void TextureCache<key_type>::clearAll() {
      typename std::map<key_type, texTuple*>::iterator i = cached.begin();
      while (i != cached.end()) {
        GLuint tid = (i->second->texId);
        glDeleteTextures(1, &tid);
        delete i->second;
        i++;
      }
      cached.clear();
    }

  template <typename key_type>
    void TextureCache<key_type>::status() {
      std::cout << "* " << m_name << " status: " << cached.size() << " textures total" << std::endl
        << "position = game_id : usage_count" << std::endl;
      printStats();
    }

  template <typename key_type>
    void TextureCache<key_type>::sink() {
      typename std::map<key_type, texTuple*>::iterator i = cached.begin();
      while (i != cached.end()) {
        if (i->second->refCount <= 1)
          i->second->refCount = 0;
        else if (i->second->refCount < _max_4)
          i->second->refCount = i->second->refCount >> 1;
        else if (i->second->refCount < _max_2) {
          INFO << m_name << " texture id " << int(i->first) << 
            " -- half-count reached" << std::endl;
          i->second->refCount = i->second->refCount >> 2;
        }
        else {
          WARN << m_name << " texture id " << int(i->first) << 
            " -- going critical" << std::endl;
          i->second->refCount = i->second->refCount >> 3;
        }
        i++;
      }
    }

  template <typename key_type>
    void TextureCache<key_type>::clear() {
      if (clearMagic == 0)
        return;
      if (cached.size() < minClearElements)
        return;
      typename std::map<key_type, texTuple*>::iterator i = cached.begin();
      uint32_t numCleared = 0;
      while (i != cached.end()) {
        if (i->second->refCount < clearMagic) {
          //INFO <<"## " << m_name << " clearing: " << int(i->first) << " count: " << i->second->refCount << std::endl;
          GLuint tid = (i->second->texId);
          glDeleteTextures(1, &tid);
          delete i->second;
          cached.erase(i);
          numCleared++;
        }
        i++;
      }
      INFO << m_name << " " << numCleared << " textures recycled" << std::endl;
    }

  template <typename key_type>
    void TextureCache<key_type>::clearStats() {
      typename std::map<key_type, texTuple*>::iterator i = cached.begin();
      while (i != cached.end()) {
        i->second->refCount = 0;
        i++;
      }
    }

  template <typename key_type>
    void TextureCache<key_type>::printStats() {
      typename std::map<key_type, texTuple*>::iterator i = cached.begin();
      size_t c = 1;
      size_t c_active = 0;
      while (i != cached.end()) {
        if (i->second->refCount > 0) {
          std::cout << c << " = " << uint32_t(i->first) << " : " << i->second->refCount << std::endl; 
          c_active++;
        }
        i++;
        c++;
      }
      std::cout << c_active << " different textures used" << std::endl;
    }

  template <typename key_type>
    GLuint TextureCache<key_type>::getTextureWithId(key_type id) {
      if (matchingCachedQuery(id)) {
        last_query_result->refCount++;
        return last_query_result->texId;
      }
      typename std::map<key_type, texTuple*>::iterator i = cached.find(id);
      if (i == cached.end()) {
        ERROR << m_name << " failed to find texture " << int(id) << std::endl;
        return 0;
      }
      else {
        cacheQuery(id, i->second);
        i->second->refCount++;
      }
      /*
       * if (i->second->isAnimated) {
       AnimControl->lookup(i->second)
       * }
       */
      return i->second->texId;
    }

  template <typename key_type>
    bool TextureCache<key_type>::hasTexture(key_type id) {
      if (matchingCachedQuery(id))
        return true; // last_query_result;
      typename std::map<key_type, texTuple*>::iterator i = cached.find(id);
      if (i == cached.end())
        return false;
      cacheQuery(id, i->second);
      return true;
    }

  template <typename key_type>
    void TextureCache<key_type>::setToAlpha(key_type id) {
      typename std::map<key_type, texTuple*>::iterator i = cached.find(id);
      if (i == cached.end()) {
        ERROR << m_name << " texture not found when trying to set alpha" << std::endl;
        return;
      }
      i->second->hasAlpha = true;
    }

  template <typename key_type>
    void TextureCache<key_type>::setToAnimated(key_type id) {
      typename std::map<key_type, texTuple*>::iterator i = cached.find(id);
      if (i == cached.end()) {
        ERROR << m_name << " texture not found when trying to set animation" << std::endl;
        return;
      }
      i->second->isAnimated = true;
    }

  template <typename key_type>
    void TextureCache<key_type>::addTexture(key_type id, GLuint texId) {
      /*
         std::map<uint8_t, texTuple*>::iterator i = cached.find(id);
         if (i == cached.end())
         return;*/
      texTuple* tt = new texTuple();
      tt->texId = texId;
      tt->refCount = 1;
      tt->hasAlpha = false;
      tt->isAnimated = false;
      cached[id] = tt;
      INFO << m_name << " GL texture " << texId << " added for key: " << int(id) << std::endl;
    }

  template <typename key_type>
    void TextureCache<key_type>::cacheQuery(key_type id, texTuple *pos) {
      has_cached_query = true;
      last_query_id = id;
      last_query_result = pos;
    }

  template <typename key_type>
    bool TextureCache<key_type>::matchingCachedQuery(key_type id) {
      return ((has_cached_query) && (id == last_query_id));
    }

  template <typename key_type>
    void TextureCache<key_type>::setClearMagic(uint32_t removeLesser) {
      clearMagic = removeLesser;
    }

  template <typename key_type>
    void TextureCache<key_type>::setMinClearElements(uint32_t minElements) {
      minClearElements = minElements;
    }

  template <typename key_type>
    unsigned int TextureCache<key_type>::instance_count = 0;

  std::numeric_limits<uint32_t> _countInfo;
  template <typename key_type>
    const uint32_t TextureCache<key_type>::_max_4 = _countInfo.max() / 4;
  template <typename key_type>
    const uint32_t TextureCache<key_type>::_max_2 = _countInfo.max() / 2;
  
  template class TextureCache<uint8_t>;
  template class TextureCache<char>;
  template class TextureCache<uint16_t>;
  template class TextureCache<uint32_t>;
}
