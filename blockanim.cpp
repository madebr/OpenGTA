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
#include "blockanim.h"

namespace OpenGTA {
  BlockAnimCtrl::BlockAnimCtrl(const DataAnimVector & v) {
    for (size_t i = 0; i < v.size(); i++) {
      OpenGTA::GraphicsBase::LoadedAnim *da = v[i];
      anims.push_back(new BlockAnim(da));
    }
   /* while (i != v.end()) {
      OpenGTA::GraphicsBase::LoadedAnim * da = *i;
      anims.push_back(new BlockAnim(da));
      i++;
    }*/
  
  };

  BlockAnim* BlockAnimCtrl::getAnim(uint8_t area, uint8_t id) {
    for (size_t i = 0; i < anims.size(); i++) {
      if ((anims[i]->ad_ptr->which == area) && (anims[i]->ad_ptr->block == id)) {
        return anims[i];
      }
    }
    return NULL;
  }

  void BlockAnimCtrl::update(uint32_t ticks) {
    for (size_t i = 0; i < anims.size(); i++)
      anims[i]->update(ticks);
  }

  BlockAnimCtrl::~BlockAnimCtrl() {
    for (size_t i = 0; i < anims.size(); i++) {
      delete anims[i];
    }
    anims.clear();
  }
}
