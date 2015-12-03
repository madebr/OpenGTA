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

#define SLOPE_RAW_DATA  OpenGTA::BlockDataHolder::Instance().slope_raw_data
#define SLOPE_TEX_DATA  OpenGTA::BlockDataHolder::Instance().slope_tex_data
#define LID_NORMAL_DATA OpenGTA::BlockDataHolder::Instance().lid_normal_data


#endif
