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
#include "animation.h"
#include "m_exceptions.h"

namespace Util {
  Animation::Animation(uint16_t num, uint16_t fps) :
    callback() {
    status = STOPPED;
    numFrames = num;
    currentFrame = 0;
    delay = 1000 / fps;
    lastChangeTicks = 0;
  }

  Animation::Animation(const Animation & other) {
    status = other.status;
    onDone = other.onDone;
    numFrames = other.numFrames;
    currentFrame = other.currentFrame;
    delay = other.delay;

    lastChangeTicks = other.lastChangeTicks;
    callback = other.callback;
  }

  void Animation::update(const uint32_t & nowTicks) {
    if (status == STOPPED)
      return;
    if (lastChangeTicks == 0)
      lastChangeTicks = nowTicks;
    if (nowTicks < lastChangeTicks + delay)
      return;
    lastChangeTicks = nowTicks;
    if (status == PLAY_FORWARD)
      flipFrame(true);
    else if (status == PLAY_BACKWARD)
      flipFrame(false);
  }

  void Animation::flipFrame(bool forward = true) {
    if (forward) {
      if (currentFrame < numFrames - 1)
        ++currentFrame;
      else if (currentFrame == numFrames - 1)
        isDone();
    } else {
      if (currentFrame == 0)
        isDone();
      else
        --currentFrame;
    }
  }

  void Animation::jumpToFrame(const uint16_t num, const Status andDo) {
    if (num >= numFrames)
      throw E_OUTOFRANGE(std::to_string(num)
                         + " >= " + std::to_string(numFrames));
    currentFrame = num;
    status = andDo;
  }

  void Animation::isDone() {
    if (onDone == STOP) {
      status = STOPPED;
      return;
    }
    if (onDone == REVERSE) {
      status = (status == PLAY_FORWARD) ? PLAY_BACKWARD : PLAY_FORWARD;
      return;
    }
    if (onDone == LOOP) {
      if (status == PLAY_FORWARD)
        jumpToFrame(0, PLAY_FORWARD);
      else if (status == PLAY_BACKWARD)
        jumpToFrame(numFrames - 1, PLAY_BACKWARD);
      return;
    }
    status = STOPPED;
    if (onDone == FCALLBACK) {
      if (callback)
        callback();
      else
        ERROR << "Wanted to call callback, but nobody was there" << std::endl;
    }
  }
}

void AnimCallback() {
  WARN << "EmptyAnimCallback called" << std::endl;
}
