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
#include "train_system.h"
#include "log.h"

namespace OpenGTA {
/*
using Util::CellIterator;
  TrainSystem::TrainSystem(std::list<TrainSegment> & list) :
    stations(),
    segments(list) {
    numTrains = 0;
  }

  TrainSystem::~TrainSystem() {
    stations.clear();
  }

  void TrainSystem::update(Uint32 now_ticks) {
  }

  void TrainSystem::loadStations(Map & map) {
    std::list<CellIterator> stationsWithTrain;
    for (int y = 0; y < 256; ++y) {
      for (int x = 0; x < 256; ++x) {
        PHYSFS_uint16 num_blocks = map.getNumBlocksAtNew(x, y);
        for (int c=0; c < num_blocks; ++c) {
          OpenGTA::Map::BlockInfo* bi = map.getBlockAtNew(x, y, c);
          assert(bi);
          if (bi->railway() && bi->railStation()) {
            stations.push_back(Util::CellIterator(map, x, y, c));
            if (bi->railStationTrain())
              stationsWithTrain.push_back(CellIterator(map, x, y, c));
          }
        }
      }
    }
    INFO << "should create " << stationsWithTrain.size() << " / " << stations.size()
      << " trains" << std::endl;
    while (stationsWithTrain.size() > 0) {
      CellIterator & i = stationsWithTrain.front();
      stationsWithTrain.pop_front();
      INFO << "at " << i.x << " " << i.y << " " << i.z << std::endl;
      TrainSegment ts(i);
      ts.trainId = numTrains++;
      segments.push_back(ts);
    }
  }
  */
}
