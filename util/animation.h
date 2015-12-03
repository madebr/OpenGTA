#ifndef UTIL_ANIMATION_H
#define UTIL_ANIMATION_H
#include <vector>
#include "Functor.h"
#include "log.h"

namespace Util {
  
    class Animation {
    public:
      typedef enum {
        STOPPED = 0,
        PLAY_FORWARD,
        PLAY_BACKWARD
      } Status;
      typedef enum {
        STOP = 0,
        REVERSE,
        LOOP,
        FCALLBACK
      } OnDone;
      Animation(uint16_t numFrames, uint16_t fps);
      Animation(const Animation & o);
      inline const uint16_t & getCurrentFrameNumber() { return currentFrame; }
      inline void set(const Status doThis, const OnDone done = STOP) { status = doThis; onDone = done; }
      inline const Status & get() { return status; }
      void jumpToFrame(const uint16_t num, const Status andDo);
      void update(const uint32_t & nowTicks);
      typedef Loki::Functor<void> CallbackType;
      void setCallback(CallbackType & cb) { callback = cb; }

      uint16_t currentFrame;
      uint16_t numFrames;
      uint32_t delay;
    protected:
      
      void flipFrame(bool forward);
      void isDone();
      Status   status;
      OnDone   onDone;
      uint32_t lastChangeTicks;

      CallbackType callback;
  };
}

#endif
