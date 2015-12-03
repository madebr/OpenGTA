#include <SDL_sound.h>
#include <SDL_mixer.h>
#include "sound_device.h"
#include "m_exceptions.h"
#include "log.h"

namespace Audio {
  SoundDevice::SoundDevice() {
    memset(cardName, 0, sizeof(cardName));
    rate = 44100;
    channels = 2;
    bufSize = 4096;
    format = AUDIO_S16;
    status = CLOSED;
  }

  SoundDevice::~SoundDevice() {
    Mix_HaltGroup(-1);
    Mix_HaltMusic();
    close();
  }

  void SoundDevice::close() {
    if (!status)
      return;
    Mix_CloseAudio();
    status = CLOSED;
  }

  void SoundDevice::open() {
    if (status)
      close();
    if (Mix_OpenAudio(rate, format, channels, bufSize))
      throw E_NOTSUPPORTED(SDL_GetError());
    SDL_AudioDriverName (cardName, sizeof (cardName));
    if (!Mix_QuerySpec(&rate, &format, &channels))
      throw E_NOTSUPPORTED(SDL_GetError());
    INFO << "Sound-device [" << cardName << "] opened at " << rate << " Hz " <<
      (format & 0xff) << " bit " << channels << " channels " << bufSize << 
      " buffer size" << std::endl;

    status = OPEN;
  }

  void SoundDevice::open(int r, Uint16 f, int c, int bs) {
    rate = r;
    format = f;
    channels = c;
    bufSize = bs;
    open();
  }
}
