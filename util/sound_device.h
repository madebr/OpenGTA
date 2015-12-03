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
#ifndef UTIL_SOUND_DEVICE_H
#define UTIL_SOUND_DEVICE_H

#ifdef WITH_SOUND

#include <SDL_stdinc.h>

namespace Audio {
  class SoundDevice {
    public:
      SoundDevice();
      ~SoundDevice();
      void close();
      void open();
      void open(int _rate, Uint16 _format, int _channels, int _bufsize);
      inline const char* getCardName() { return cardName; }
      inline const int   getRate() { return rate; }
      inline const int   getNumChannels() { return channels; }
      inline const int   getBufferSize() { return bufSize; }
      inline const Uint16 getFormat() { return format; }
      enum Status {
        CLOSED = 0,
        OPEN,
        ERROR,
      };
      inline Status getStatus() const { return status; }
    private:
      char   cardName[50];
      int    rate;
      int    channels;
      int    bufSize;
      Uint16 format;
      Status status;
  };
}
#endif

#endif
