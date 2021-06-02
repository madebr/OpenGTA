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
  BlockAnimCtrl::BlockAnimCtrl(const std::vector<GraphicsBase::LoadedAnim*> & v) {
    for (const auto &anim : v) {
      anims_.emplace_back(anim);
    }
   /* while (i != v.end()) {
      OpenGTA::GraphicsBase::LoadedAnim * da = *i;
      anims.push_back(new BlockAnim(da));
      i++;
    }*/
  
  };

  std::optional<BlockAnim> BlockAnimCtrl::getAnim(uint8_t area, uint8_t id)
  {
    for (const auto &anim : anims_) {
      if ((anim.ad_ptr->which == area) && (anim.ad_ptr->block == id))
        return anim;
    }
    return std::nullopt;
  }

  void BlockAnimCtrl::update(uint32_t ticks)
  {
    for (auto &anim : anims_) {
      anim.update(ticks);
    }
  }
}
