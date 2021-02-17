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
#include <SDL.h>
#include <iostream>
#include "timer.h"

Timer::TimeEvent::TimeEvent(const uint32_t & b, const uint32_t e, CallbackType & c) :
  begin(b), end(e), callback(c) {}

Timer::TimeEvent::TimeEvent(const uint32_t & b, CallbackType & c) :
  begin(b), end(b), callback(c) {}

Timer::TimeEvent::TimeEvent(const TimeEvent & o) :
  begin(o.begin), end(o.end), callback(o.callback) {}

Timer::Timer() {
  simIsRunning = false;
#ifdef TIMER_OPENSTEER_CLOCK
  sdlTicks = uint32_t(floor(clock.realTimeSinceFirstClockUpdate()*1000));
  clock.setPausedState(simIsRunning);
#else
  sdlTicks = SDL_GetTicks();
#endif
  simTicks = 0;
  delta = 0;
}

Timer::~Timer() {
  clearAllEvents();
}


void Timer::update() {
#ifdef TIMER_OPENSTEER_CLOCK
  if (simIsRunning)
    clock.update();
  uint32_t nowTicks = uint32_t(floor(clock.realTimeSinceFirstClockUpdate()*1000));
#else
  uint32_t nowTicks = SDL_GetTicks();
#endif
  delta = nowTicks - sdlTicks;
  sdlTicks = nowTicks;
  if (simIsRunning)
    simTicks += delta;

  if (realTimeEvents.size() > 0)
    checkRTEvents();

  if (simIsRunning && simTimeEvents.size() > 0)
    checkSimEvents();
}

void Timer::setSimulationRunning(bool yes) {
  simIsRunning = yes;
#ifdef TIMER_OPENSTEER_CLOCK
  clock.setPausedState(!yes);
#endif
}

void Timer::checkRTEvents() {
  RealTimeMap::iterator i = realTimeEvents.begin();
  while (i != realTimeEvents.end() && i->first <= sdlTicks) {
    TimeEvent & te = i->second;
    float as_float = 0.0f;
    bool doRemove = false;
    if (te.begin == te.end) {
      doRemove = true;
    }
    else {
      if (sdlTicks > te.end) {
        as_float = 1.0f;
        doRemove = true;
      }
      else
        as_float = float(sdlTicks - te.begin) / float(te.end - te.begin);
    }
    te.callback(as_float);
    if (doRemove) {
      RealTimeMap::iterator j = i++;
      realTimeEvents.erase(j);
    }
    else
      ++i;
  }
}

void Timer::checkSimEvents() {
  SimTimeMap::iterator i = simTimeEvents.begin();
  while (i != simTimeEvents.end() && i->first <= simTicks) {
    std::cout << "event start: " << i->first << " now " << simTicks << std::endl;
    TimeEvent & te = i->second;
    float as_float = 0.0f;
    bool doRemove = false;
    if (te.begin == te.end) {
      doRemove = true;
    }
    else {
      if (simTicks > te.end) {
        as_float = 1.0f;
        doRemove = true;
      }
      else
        as_float = float(simTicks - te.begin) / float(te.end - te.begin);
    }
    te.callback(as_float);
    if (doRemove) {
      SimTimeMap::iterator j = i++;
      simTimeEvents.erase(j);
    }
    else
      ++i;
  }
}

void Timer::registerCallback(bool simTime, CallbackType & c, const uint32_t & b, const uint32_t & e) {
  if (simTime)
    simTimeEvents.insert(std::make_pair(b, TimeEvent(b, e, c)));
  else
    realTimeEvents.insert(std::make_pair(b, TimeEvent(b, e, c)));
}

void Timer::registerCallback(bool simTime, CallbackType & c, const uint32_t & b) {
  if (simTime)
    simTimeEvents.insert(std::make_pair(b, TimeEvent(b, c)));
  else
    realTimeEvents.insert(std::make_pair(b, TimeEvent(b, c)));
}

void Timer::clearAllEvents() {
  realTimeEvents.clear();
  simTimeEvents.clear();
}


#if 0
void test1(float a) {
  std::cout << "test1: " << a << std::endl;
}

int main() {
  SDL_Init(SDL_INIT_TIMER);

  SDL_Delay(10);
  Timer * t = new Timer;
  t->setSimulationRunning(true);
  Timer::CallbackType cb(test1);
  uint32_t target = t->getTime() + 100;
  uint32_t finish = target + 100;
  t->registerCallback(true, cb, target, finish);

  while (t->getRealTime() < target + 1000) {
    t->update();
    SDL_Delay(10);
  }

/*
  Timer * t = new Timer;
  if (t->getRealTime() < 8 || t->getRealTime() > 12)
    std::cout << "expected 10 got " << t->getRealTime() << std::endl;
  else
    std::cout << "got ~10: " << t->getRealTime() << std::endl;

  Timer::CallbackType cb(test1);

  uint32_t target = t->getRealTime() + 100;
  uint32_t finish = target + 100;
  t->registerCallback(cb, target, finish);

  uint32_t slept = 0;
  while (t->getRealTime() < target + 200) {
    t->update();
    std::cout << "now: " << t->getRealTime() << std::endl;

    SDL_Delay(10);
  }
*/

  delete t;
}
#endif
