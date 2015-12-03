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
#ifndef OBJECT_CONTROLLER_H
#define OBJECT_CONTROLLER_H

#include <stdint.h>
#include "set.h"

namespace OpenGTA {
  class EntityController {
    public:
      EntityController();

      void zero();
      typedef uint32_t Storage_T;
      void setRaw(Storage_T v); 
      inline const Storage_T & getRaw() { return rawData; }
    protected:
      EntityController(const EntityController & other);
      Storage_T rawData;
      Util::Set    dataSet;
  };

  class ControllerWithMemory : public EntityController {
    public:
      ControllerWithMemory() : EntityController(),
        lastRawData(rawData) {}
      ControllerWithMemory(const ControllerWithMemory & o) : 
        EntityController(o), lastRawData(o.lastRawData) {}
      bool statusChanged(); 
    protected:
      Storage_T lastRawData;
  };

  class Pedestrian;
//  class PedController : public EntityController {
  class PedController : public ControllerWithMemory {
    public:
      PedController() {}
//      PedController(const PedController & other) : EntityController(other) {}
      PedController(const PedController & other) : ControllerWithMemory(other) {}
      void setTurnLeft(bool press = true);
      inline void releaseTurnLeft() { setTurnLeft(false); }
      void setTurnRight(bool press = true);
      inline void releaseTurnRight() { setTurnRight(false); }
      signed char getTurn();
      void setMoveForward(bool press = true);
      inline void releaseMoveForward() { setMoveForward(false); }
      void setMoveBack(bool press = true);
      inline void releaseMoveBack() { setMoveBack(false); }
      signed char getMove();
      void setAction(bool press = true);
      inline void releaseAction() { setAction(false); }
      bool getAction();
      // bool getAction();
      void setJump(bool press = true);
      inline void releaseJump() { setJump(false); }
      bool getJump();
      // bool getJump();
      void setFireWeapon(bool press = true);
      inline void releaseFireWeapon() { setFireWeapon(false); }
      bool getFireWeapon();

      void setActiveWeapon(unsigned char);
      unsigned char getActiveWeapon();

      void setRunning(bool yes = true);
      inline void releaseRunning() { setRunning(false); }
      bool getRunning();

      // weapons, equip, shoot
  };

  class VehicleController : public EntityController {
    public:
      VehicleController() {}
      VehicleController(const VehicleController & other) : EntityController(other) {}
      void setTurnLeft(bool press = true);
      inline void releaseTurnLeft() { setTurnLeft(false); }
      void setTurnRight(bool press = true);
      inline void releaseTurnRight() { setTurnRight(false); }
      void setAccelerate(bool press = true);
      inline void releaseAccelerate() { setAccelerate(false); }
      void setReverse(bool press = true);
      inline void releaseReverse() { setReverse(false); }
      void setHandbrake(bool press = true);
      inline void releaseHandbrake() { setHandbrake(false); }
      void setAction(bool press = true);
      inline void releaseAction() { setAction(false); }
      
      // vehicle specials
  };

  // HeliController?
}
#endif
