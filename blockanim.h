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
#include "opengta.h"
#include "animation.h"

#include <optional>

namespace OpenGTA {
  class BlockAnim : public Util::Animation {
    public:
      BlockAnim(GraphicsBase::LoadedAnim * anim_data) : 
        // fix for STYLE001.G24 water anim seems one frame longer than data exists!
        Util::Animation(
          (anim_data->frameCount == 11 && anim_data->which == 1 ? anim_data->frameCount : anim_data->frameCount + 1),
           5),
        ad_ptr(anim_data) {
        set(PLAY_FORWARD, LOOP);
        /*
        INFO << "ANIM: " << int(anim_data->block) << " " << int(anim_data->which)<< std::endl;
        for (int i= 0; i < anim_data->frameCount; i++) {
          INFO << "FRAME " << int(anim_data->frame[i]) << std::endl;
        }*/
      }
      inline uint8_t getFrame(uint8_t num) const noexcept {
        return ad_ptr->frame[num];
      }
      GraphicsBase::LoadedAnim * ad_ptr;
  };

  class BlockAnimCtrl {
    public:
      BlockAnimCtrl(const std::vector<GraphicsBase::LoadedAnim*> & v);
      void update(uint32_t ticks);
      std::optional<BlockAnim> getAnim(uint8_t area, uint8_t id);
    private:
      std::vector<BlockAnim> anims_;

  };
}
