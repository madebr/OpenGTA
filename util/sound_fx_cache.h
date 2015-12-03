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
#ifndef SOUND_FX_CACHE_H
#define SOUND_FX_CACHE_H
#ifdef WITH_SOUND

#include <map>
#include <string>
#include <SDL_mixer.h>
#include "fx_sdt.h"

namespace Audio {

  struct ChunkId {
    std::string src_file;
    size_t      idx_in_file;
    ChunkId(const std::string & file, const size_t idx) :
      src_file(file), idx_in_file(idx) {}
    bool operator == (const ChunkId & o) const {
      return (idx_in_file == o.idx_in_file && src_file == o.src_file);
    }
    bool operator < (const ChunkId & o) const {
      if (idx_in_file < o.idx_in_file)
        return true;
      if (idx_in_file > o.idx_in_file)
        return false;
      return src_file < o.src_file;
    }
  };

  struct ChunkData {
    Uint8 * mem_buf;
    Mix_Chunk * chunk;
    size_t ref;
    ChunkData(Uint8 * m, Mix_Chunk * c, size_t r = 1) :
      mem_buf(m), chunk(c), ref(r) {}
    ChunkData(const ChunkData & o) :
      mem_buf(o.mem_buf), chunk(o.chunk), ref(o.ref) {}
  };

  class AudioChunkCache {
    public:
      typedef std::map< ChunkId, ChunkData > CacheType;
      CacheType cached;
      ChunkData & getChunk(std::string & file, size_t idx);
      OpenGTA::SoundsDB * getDB(std::string db_file);
    private:
      void prepareDB(std::string db_file);
      ChunkData loadChunk(std::string & file, size_t idx);
      typedef std::map< std::string, OpenGTA::SoundsDB* > LookupCache;
      LookupCache lookup;
  };
}
#endif

#endif
