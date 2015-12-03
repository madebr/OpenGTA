#include <SDL.h>
#include "timer.h"
#include <iostream>

Timer::TimeEvent::TimeEvent(const uint32_t & b, const uint32_t e, CallbackType & c) :
  begin(b), end(e), callback(c) {}

Timer::TimeEvent::TimeEvent(const uint32_t & b, CallbackType & c) :
  begin(b), end(b), callback(c) {}

Timer::TimeEvent::TimeEvent(const TimeEvent & o) :
  begin(o.begin), end(o.end), callback(o.callback) {}

Timer::Timer() {
  sdlTicks = SDL_GetTicks();
  simTicks = 0;
  delta = 0;
  simIsRunning = false;
}

Timer::~Timer() {
  clearAllEvents();
}

void Timer::update() {
  uint32_t nowTicks = SDL_GetTicks();
  delta = nowTicks - sdlTicks;
  sdlTicks = nowTicks;
  if (simIsRunning)
    simTicks += delta;

  if (realTimeEvents.size() > 0)
    checkRTEvents();

  if (simIsRunning && simTimeEvents.size() > 0)
    checkSimEvents();
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
    simTimeEvents.insert(std::make_pair<uint32_t, TimeEvent>(b, TimeEvent(b, e, c)));
  else
    realTimeEvents.insert(std::make_pair<uint32_t, TimeEvent>(b, TimeEvent(b, e, c)));
}

void Timer::registerCallback(bool simTime, CallbackType & c, const uint32_t & b) {
  if (simTime)
    simTimeEvents.insert(std::make_pair<uint32_t, TimeEvent>(b, TimeEvent(b, c)));
  else
    realTimeEvents.insert(std::make_pair<uint32_t, TimeEvent>(b, TimeEvent(b, c)));
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
