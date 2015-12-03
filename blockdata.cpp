#include "blockdata.h"
namespace OpenGTA {
  float BlockData::slope_raw_data[numBlockTypes][numFaces][4][3] = {
#include "slope1_data.h"
  };
  float BlockData::slope_tex_data[numBlockTypes][numFaces-1][4][2] = {
#include "slope1_tcoords.h"
  };
  float BlockData::lid_normal_data[numBlockTypes][3] = { 
#include "lid_normal_data.h"
  };
}
