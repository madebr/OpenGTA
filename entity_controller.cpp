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
#include "entity_controller.h"
#include "log.h"
#include "m_exceptions.h"

namespace OpenGTA {
  EntityController::EntityController() :
    rawData(0),
    dataSet(sizeof(rawData) * 8, (unsigned char*)&rawData) {}

  EntityController::EntityController(const EntityController & other) :
    rawData(other.rawData),
    dataSet(sizeof(rawData) * 8, (unsigned char*)&rawData) {}

  void EntityController::zero() {
    rawData = 0;
  }

  void EntityController::setRaw(Storage_T v) {
    rawData = v;
  }

  bool ControllerWithMemory::statusChanged() {
    bool res = (rawData != lastRawData);
    if (res)
      lastRawData = rawData;
    return res; 
  }

  void PedController::setTurnLeft(bool press) {
    dataSet.set_item(0, press);
  }

  void PedController::setTurnRight(bool press) {
    dataSet.set_item(1, press);
  }

  void PedController::setMoveForward(bool press) {
    dataSet.set_item(2, press);
  }

  void PedController::setMoveBack(bool press) {
    dataSet.set_item(3, press);
  }

  void PedController::setAction(bool press) {
    dataSet.set_item(4, press);
  }

  void PedController::setJump(bool press) {
    dataSet.set_item(5, press);
  }

  void PedController::setFireWeapon(bool press) {
    dataSet.set_item(6, press);
  }

  signed char PedController::getTurn() {
    if (dataSet.get_item(0) && dataSet.get_item(1)) // special: straight ahead
      return 0;
    if (dataSet.get_item(0))
      return 1;
    else if (dataSet.get_item(1))
      return -1;
    return 0;
  }

  signed char PedController::getMove() {
    if (dataSet.get_item(2) && dataSet.get_item(3)) // special: evens out
      return 0;
    if (dataSet.get_item(2))
      return 1;
    else if (dataSet.get_item(3))
      return -1;
    return 0;
  }

  bool PedController::getAction() {
    return dataSet.get_item(4);
  }

  bool PedController::getJump() {
    return dataSet.get_item(5);
  }

  bool PedController::getFireWeapon() {
    return dataSet.get_item(6);
  }

  void PedController::setRunning(bool yes) {
    dataSet.set_item(7, yes);
  }

  bool PedController::getRunning() {
    return dataSet.get_item(7);
  }

  unsigned char PedController::getActiveWeapon() {
    // 0 .. k
    unsigned char r = 0;
    for (int j = 0; j < 3; ++j) {
      if (dataSet.get_item(j + 8))
        r += (1 << j);
    }
    return r;
  }

  void PedController::setActiveWeapon(unsigned char k) {
    if (k > 7) {
      throw E_OUTOFRANGE("foo");
    }
    for (int j = 0; j < 3; ++j) {
      if (k & 1u)
        dataSet.set_item(j + 8, true);
      else
        dataSet.set_item(j + 8, false);
      k >>= 1;
    }
  }

#if 0
#include <SDL_events.h>
#include "localplayer.h"
class ConfigurableKeyConsumer {
    static const SDLKey sym_array[];
    static const int TURN_LEFT = 0;
    static const int TURN_RIGHT = 1;
    static const int MOVE_FORWARD = 2;
    static const int MOVE_BACKWARD = 3;
  public:
    void handle(const SDL_keysym & ks, bool press) {
      PedController & pc = LocalPlayer::Instance().getCtrl();
      if (ks.sym == sym_array[TURN_LEFT]) {
        pc.setTurnLeft(true);
        return;
      }
      else if (ks.sym == sym_array[TURN_RIGHT]) {
        pc.setTurnRight(true);
        return;
      }
    }
};
#endif

#if 0
#include <SDL_events.h>
#include "localplayer.h"

  template <class ENTITY>
  class ClassicKeyConsumer {
    public:
      bool handle(const SDL_keysym & ks, bool press);
  };

  template <> bool ClassicKeyConsumer<PedController>::handle(const SDL_keysym & ks, bool press) {
    bool swallow_event = true;
    PedController & pc = LocalPlayer::Instance().getEntity().m_control;
    switch(ks.sym) {
      case SDLK_LEFT:
        pc.setTurnLeft(press);
        break;
      case SDLK_RIGHT:
        pc.setTurnRight(press);
        break;
      case SDLK_UP:
        pc.setMoveForward(press);
        break;
      case SDLK_DOWN:
        pc.setMoveBack(press);
        break;
      case SDLK_RETURN:
        pc.setAction(press);
        break;
      case SDLK_SPACE:
        pc.setJump(press);
        break;
      default:
        swallow_event = false;
        break;
    }
    return swallow_event;
  }
#endif
}
