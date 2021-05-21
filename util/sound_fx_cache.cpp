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
#ifdef WITH_SOUND
#include "sound_fx_cache.h"
#include "sound_resample2.h"

namespace Audio {
  ChunkData & AudioChunkCache::getChunk(std::string & file, size_t idx) {
    ChunkId id(file, idx);
    CacheType::iterator i = cached.find(id);
    if (i == cached.end()) {
      ChunkData c = loadChunk(file, idx);
      cached.insert(std::make_pair(id, c));
      i = cached.find(id);
    }
    return i->second;
  }

  ChunkData AudioChunkCache::loadChunk(std::string & file, size_t idx) {
    LookupCache::iterator j = lookup.find(file);
    if (j == lookup.end()) {
      prepareDB(file);
      j = lookup.find(file);
    }
    OpenGTA::SoundsDB & db = *j->second;
    OpenGTA::SoundsDB::Entry & e = db.getEntry(idx);
    size_t si;
    unsigned char* mem = db.getBuffered(idx);
    Uint8 *mem2 = NULL;
    if (file.find("000") != std::string::npos) {
      mem2 = (Uint8*)Audio::resample16(mem, e.rawSize, si, e.sampleRate, 44100);
    }
    else
      mem2 = (Uint8*)Audio::resample_new(mem, e.rawSize, si, e.sampleRate, 44100);
    Mix_Chunk * music = Mix_QuickLoad_RAW(mem2, si);
    return ChunkData(mem2, music, 1);
  }

  void AudioChunkCache::prepareDB(std::string db) {
    lookup[db] = new OpenGTA::SoundsDB(db);
  }

}
#endif
