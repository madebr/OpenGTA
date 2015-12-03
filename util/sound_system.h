#ifndef SOUND_SYSTEM_H
#define SOUND_SYSTEM_H

#ifdef WITH_SOUND
#include "sound_device.h"
#include "sound_music_player.h"
#include "sound_fx_cache.h"
#endif

namespace Audio {
  class SoundSystem {
#ifdef WITH_SOUND
    private:
      SoundDevice device;
      AudioChunkCache chunkCache;
#endif
    public:
      SoundSystem();
      ~SoundSystem();
      void playFx(std::string file, size_t idx);
      void playMusic(std::string file);
      void listMusicDecoders();
      bool enabled;
  };
}

#endif
