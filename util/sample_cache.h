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
#ifndef SAMPLE_CACHE_H
#define SAMPLE_CACHE_H

#include <SDL_mixer.h>
#include <map>

namespace Audio {
  
  class SampleType_MixChunk {
    public:
      SampleType_MixChunk(Uint8 *mem, Mix_Chunk *ch) : memory(mem), chunk(ch) {}
      ~SampleType_MixChunk() {
        delete [] memory;
        Mix_FreeChunk(chunk);
      }
      Uint8     *memory;
      Mix_Chunk *chunk;
    private:
      SampleType_MixChunk(const SampleType_MixChunk & other);
      SampleType_MixChunk() : memory(0), chunk(0) {}
  };

  typedef size_t SampleKeyType;

  template < typename K, typename V >
    class Cache {
      public:
        ~Cache() {
          clear;
        }
        void clear();
        typedef std::map< K, V > CacheMapType;
        V operator[](K k) { return m_map[k]; }
        void add(K k, V v) { m_map.insert(std::make_pair<K, V>(k, v)); }
      private:
        CacheMapType m_map;

    };
  
  typedef Cache< SampleKeyType, SampleType_MixChunk* > SampleCache;

  template<> void SampleCache::clear() {
    for (CacheMapType::iterator i = m_map.begin(); i != m_map.end(); i++) {
      delete i->second;
    }
    m_map.clear();
  }
}

#endif
