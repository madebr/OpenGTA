#ifndef OGTA_TIMER_H
#define OGTA_TIMER_H
#include <map>
#include "Singleton.h"
#include "Functor.h"

class Timer {
  public:
    typedef Loki::Functor<void, LOKI_TYPELIST_1(float)> CallbackType;
    struct TimeEvent {
      TimeEvent(const uint32_t & b, const uint32_t e, CallbackType & c);
      TimeEvent(const uint32_t & b, CallbackType & c);
      TimeEvent(const TimeEvent & o);
      const uint32_t begin;
      const uint32_t end;
      CallbackType callback;
    };
    Timer();
    ~Timer();
    // simulation time in ticks
    inline const uint32_t & getTime() const { return simTicks; }
    // time since SDL_Init
    inline const uint32_t & getRealTime() const { return sdlTicks; }
    void update();
    inline void setSimulationRunning(bool yes) { simIsRunning = yes; }
    inline bool getSimulationRunning() const { return simIsRunning; }
    void registerCallback(bool simTime, CallbackType & c, const uint32_t & b, const uint32_t & e);
    void registerCallback(bool simTime, CallbackType & c, const uint32_t & b);
    void clearAllEvents();
  private:
    void checkRTEvents();
    void checkSimEvents();
    uint32_t sdlTicks;
    uint32_t simTicks;
    uint32_t delta;
    bool     simIsRunning;

    typedef std::multimap<uint32_t, TimeEvent> RealTimeMap;
    RealTimeMap realTimeEvents;
    typedef RealTimeMap SimTimeMap;
    SimTimeMap simTimeEvents;
};

typedef Loki::SingletonHolder<Timer, Loki::CreateUsingNew, 
  Loki::DefaultLifetime, Loki::SingleThreaded> TimerHolder;

#endif
