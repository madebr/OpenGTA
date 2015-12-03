#include "localplayer.h"
#include "spritemanager.h"

namespace OpenGTA {
  Pedestrian & PlayerController::getPed() {
    return SpriteManagerHolder::Instance().getPed(playerId); 
  }

  bool PlayerController::up(const uint32_t & key) {
    if (!pc_ptr)
      return false;
    bool handled = false;
    switch (key) {
      case SDLK_LEFT:
        pc_ptr->releaseTurnLeft();
        handled = true;
        break;
      case SDLK_RIGHT:
        pc_ptr->releaseTurnRight();
        handled = true;
        break;
      case SDLK_UP:
        pc_ptr->releaseMoveForward();
        handled = true;
        break;
      case SDLK_DOWN:
        pc_ptr->releaseMoveBack();
        handled = true;
        break;
      case SDLK_LCTRL:
        pc_ptr->releaseFireWeapon();
        handled = true;
        break;
      default:
        break;
    }
    return handled;
  }

  bool PlayerController::down(const uint32_t & key) {
    if (!pc_ptr)
      return false;
    bool handled = false;
    switch(key) {
      case SDLK_LEFT:
        handled = true;
        break;
      case SDLK_RIGHT:
        handled = true;
        break;
      case SDLK_UP:
        handled = true;
        break;
      case SDLK_DOWN:
        handled = true;
        break;
      case SDLK_LCTRL:
        handled = true;
        break;
      case SDLK_LSHIFT:
        handled = true;
        break;
      default:
        break;
    }
    return handled;
  }

}
