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
#ifndef UTIL_ANIMATION_H
#define UTIL_ANIMATION_H
#include <functional>
#include <vector>
#include "log.h"

namespace Util {

  /** Capsules play-frames-in-sequence logic.
   *
   * This class only knows about the number of frames
   * and the fps; it does have multiple flags that
   * switch the behaviour whenever the 'last' frame is
   * finished.
   */
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
      inline const uint16_t & getCurrentFrameNumber() const { return currentFrame; }
      inline void set(const Status doThis, const OnDone done = STOP) { status = doThis; onDone = done; }
      inline const Status & get() const { return status; }
      inline const OnDone & getDone() const { return onDone; }
      void jumpToFrame(const uint16_t num, const Status andDo);
      void update(const uint32_t & nowTicks);
      using CallbackType = std::function<void()>;
      void setCallback(const CallbackType & cb) { callback = cb; }

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
