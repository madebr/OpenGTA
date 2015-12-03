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
#ifndef UTIL_KEYHANDLER_H
#define UTIL_KEYHANDLER_H
#include <stdint.h>
#include <list>
#include <vector>
#include "pf_tree.hpp"

namespace Util {

/** Interface for key listeners.
 *
 * Must implement both keypress and release functions;
 * the functions should return \em true when the input
 * value is consumed by a handler.
 */
class KeyHandler {
  public:
    virtual ~KeyHandler() {}
    virtual bool up(const uint32_t & key) = 0;
    virtual bool down(const uint32_t & key) = 0;
};

/** Keys that do _not_ control the player character.
 */
class IngameCommonKeys : public KeyHandler {
  public:
    bool up(const uint32_t & key);
    bool down(const uint32_t & key);
};

class MenuKeys : public KeyHandler {
  public:
    bool up(const uint32_t & key);
    bool down(const uint32_t & key);
};

class CheatKeys : public KeyHandler {
  public:
    bool up(const uint32_t & key);
    bool down(const uint32_t & key);
    template <typename T>
    struct Callback_Lua {
      void call(T);
    };
    typedef PrefixFreeTree::Walker<uint32_t, Callback_Lua> TreeOfCheats;

  private:
    std::vector<uint32_t> cheat_fn_refs;
};

class KeyHandlerChain {
  public:
    void up(const uint32_t & key);
    void down(const uint32_t & key);
    void addHandler(KeyHandler*, bool front = true);
    void removeHandler(KeyHandler*);
  private:
    typedef std::list<KeyHandler*> ListOfHandlers;
    ListOfHandlers activeHandlers;
};

}

#endif
