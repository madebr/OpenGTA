#include <cassert>
#include "ai.h"
#include "opengta.h"
#include "game_objects.h"
#include "cell_iterator.h"
#include "log.h"

namespace OpenGTA {
  namespace AI {
    namespace Pedestrian {
      void walk_pavement(OpenGTA::Pedestrian* ped) {
        assert(ped);
        Util::CellIterator ci(ped->pos);
        if (!ci.isValid())
          return;
        // OpenGTA::Map::BlockInfo & bi = ci.getBlock();
        //INFO << " ped in bt: " << int(bi.blockType()) << std::endl;
        //INFO << ped->pos.x << " " << ped->pos.z << std::endl;
        std::pair<bool, Util::CellIterator> f = ci.findNeighbourWithType(3, ped->rot);
        if (f.first) {
          //INFO << "next: " << f.second.x << " " << f.second.y << std::endl;
          ped->aiData.pos1 = Vector3D(f.second.x+0.5f, ped->pos.y, f.second.y+0.5f);
          ped->aiMode = 1;
        }
      }

      void moveto_shortrange(OpenGTA::Pedestrian *ped) {
        assert(ped);
        float d = Util::distance(ped->pos, ped->aiData.pos1);
        //INFO << "dist: " << d << std::endl;
        float a = Util::xz_turn_angle(ped->pos, ped->aiData.pos1);
        float da = Util::xz_angle(ped->pos, ped->aiData.pos1);
        //INFO << a << std::endl;
        //INFO << "rot " << ped->rot << std::endl;
        ped->m_control.setMoveForward(false);
        ped->m_control.setTurnLeft(false);
        ped->m_control.setTurnRight(false);
        //INFO << ped->rot + a << std::endl;
        if (fabs(ped->rot - da) > 5) {
          if (a > 0)
            ped->m_control.setTurnLeft(true);
          else
            ped->m_control.setTurnRight(true);
        }
        if (fabs(ped->rot - da) < 120 && d > 0.19f)
          ped->m_control.setMoveForward(true);
        if (d <= 0.19f)
          ped->aiMode = 0;
      }
    }
  }
}
