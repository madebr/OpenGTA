#ifndef OPENGTA_BLOCKDATA
#define OPENGTA_BLOCKDATA
#include <cstddef>
#include "Singleton.h"

namespace OpenGTA {
  struct BlockData {
    static const size_t numBlockTypes = 45;
    static const size_t numFaces = 5;

    static float slope_raw_data[numBlockTypes][numFaces][4][3];
    static float slope_tex_data[numBlockTypes][numFaces-1][4][2];
    static float lid_normal_data[numBlockTypes][3];
  };

  typedef Loki::SingletonHolder<BlockData, Loki::CreateUsingNew, 
          Loki::DefaultLifetime, Loki::SingleThreaded> BlockDataHolder;
}

#define SLOPE_RAW_DATA  BlockDataHolder::Instance().slope_raw_data
#define SLOPE_TEX_DATA  BlockDataHolder::Instance().slope_tex_data
#define LID_NORMAL_DATA BlockDataHolder::Instance().lid_normal_data


#endif
