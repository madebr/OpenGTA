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
#ifndef OGTA_TIMER_H
#define OGTA_TIMER_H
#include <functional>
#include <map>

// instead of SDL_GetTicks()
#define TIMER_OPENSTEER_CLOCK

#ifdef TIMER_OPENSTEER_CLOCK
#include "OpenSteer/Clock.h"
#endif

class Timer {
  private:
    Timer();
    ~Timer();
  public:
    Timer(const Timer& copy) = delete;
    Timer& operator=(const Timer& copy) = delete;

    static Timer& Instance()
    {
      static Timer instance;
      return instance;
    }

    using CallbackType = std::function<void(float)>;
    struct TimeEvent {
      TimeEvent(const uint32_t & b, const uint32_t e, CallbackType & c);
      TimeEvent(const uint32_t & b, CallbackType & c);
      TimeEvent(const TimeEvent & o);
      const uint32_t begin;
      const uint32_t end;
      CallbackType callback;
    };
    // simulation time in ticks
    inline const uint32_t & getTime() const { return simTicks; }
    // time since SDL_Init
    inline const uint32_t & getRealTime() const { return sdlTicks; }
    void update();
    void setSimulationRunning(bool yes);
    inline bool getSimulationRunning() const { return simIsRunning; }
    void registerCallback(bool simTime, CallbackType & c, const uint32_t & b, const uint32_t & e);
    void registerCallback(bool simTime, CallbackType & c, const uint32_t & b);
    void clearAllEvents();
#ifdef TIMER_OPENSTEER_CLOCK
    OpenSteer::Clock clock;
#endif
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

#endif
