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
    close();
  }

  void SoundDevice::close() {
    if (!status)
      return;
    Mix_HaltMusic();
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
    INFO << 
      "Sound-device [" << cardName << "], rate " << rate << " Hz, " <<
      (format & 0xff) << " bit, " << std::endl << "  " << 
      channels << " channels, " << bufSize << " buffer size" << std::endl;

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
#endif
