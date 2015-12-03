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
#include <SDL_keysym.h>
#include "key_handler.h"
#include "log.h"

namespace Util {
  bool IngameCommonKeys::up(const uint32_t & key) {
    bool handled = false;
    return handled;
  }

  bool IngameCommonKeys::down(const uint32_t & key) {
    return false;
  }

  void KeyHandlerChain::up(const uint32_t & key) {
    for (ListOfHandlers::iterator i = activeHandlers.begin();
      i != activeHandlers.end(); i++) {
      if ((*i)->up(key))
        return;
    }
  }

  void KeyHandlerChain::down(const uint32_t & key) {
    for (ListOfHandlers::iterator i = activeHandlers.begin();
      i != activeHandlers.end(); i++) {
      if ((*i)->down(key))
        return;
    }
  }

  void KeyHandlerChain::addHandler(KeyHandler * kh, bool front) {
    if (front)
      activeHandlers.push_front(kh);
    else
      activeHandlers.push_back(kh);
  }

  void KeyHandlerChain::removeHandler(KeyHandler * kh) {
    for (ListOfHandlers::iterator i = activeHandlers.begin();
      i != activeHandlers.end(); i++) {
        if (kh == *i)
          activeHandlers.erase(i);
    }
    ERROR << "not a valid handler pointer" << std::endl;
  }
}
