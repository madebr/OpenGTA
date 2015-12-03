#ifndef UTIL_SOUND_DEVICE_H
#define UTIL_SOUND_DEVICE_H

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
