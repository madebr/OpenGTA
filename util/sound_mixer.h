#ifndef UTIL_SOUND_MIXER_H
#define UTIL_SOUND_MIXER_H

#include <SDL_sound.h>
#include <SDL_mixer.h>
#include "sound_device.h"

namespace Audio {
  class Mixer {
    public:
      Mixer();
      ~Mixer();

      Audio::SoundDevice device;

      int setVolume(int channel, int volume);
      int setVolumeMusic();
      int getVolume(int channel);
      int getVolumeMusic();

      void playSample(const size_t & sampleId);

      void pause(int channel);
      void pauseMusic();
    private:
      inline bool checkDeviceOK() { return (device.getStatus() == 
        Audio::SoundDevice::OPEN); }
  };
}

#endif
