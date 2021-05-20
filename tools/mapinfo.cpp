#include <cassert>
#include "opengta.h"
#include "navdata.h"
#include "log.h"

int main(int argc, char* argv[]) {
  int x = 0;
  int y = 0;

  if (argc < 2) {
    std::cerr << "USAGE: mapinfo map_filename [x y]" << std::endl;
    return 1;
  }

  PHYSFS_init(argv[0]);
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);

  std::string map_filename(argv[1]);
  OpenGTA::Map map(map_filename);

  if (argc == 4) {
    x = atoi(argv[2]);
    y = atoi(argv[3]);
  }
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  INFO << "Querying position: " << x << ", " << y << std::endl;

  OpenGTA::NavData::Sector* sec = map.nav->getSectorAt(x, y);

  INFO << "* " << sec->name << " *" << std::endl;

  PHYSFS_uint16 num_blocks = map.getNumBlocksAt(x, y);
  OpenGTA::Map::BlockInfo* bi = NULL;
  INFO << num_blocks << " empty blocks" << std::endl;
  for (int c=6-num_blocks; c >= 1; c--) {
    std::cout << "block " << c << std::endl;
    bi = map.getBlockAt(x, y, c);
    assert(bi);
    std::cout << "moves: "<< int(bi->upOk()) << ", " << int(bi->downOk()) << ", "
    << int(bi->leftOk()) << ", " << int(bi->rightOk()) << std::endl;
    std::cout << "type: " << int(bi->blockType()) << " flat: " << 
      int(bi->isFlat()) << " slope-type: " << int(bi->slopeType()) << 
      " rot: " << int(bi->rotation()) << " remap idx: " << 
      int(bi->remapIndex()) << std::endl;
    std::cout << "textures: left " << int(bi->left) << " right " << int(bi->right) <<
    " top " << int(bi->top) << " bottom " << int(bi->bottom) << " lid " << int(bi->lid)
    << " flip t-b: " << int(bi->flipTopBottom()) << " flip l-r: " << 
    int(bi->flipLeftRight()) << std::endl;

    int v_count = 0;
    if (bi->lid)
      v_count += 3*4;
    if (bi->isFlat()) {
      if (bi->top)
        v_count += 3*4*2;
      if (bi->left)
        v_count += 3*4*2;
    }
    else {
      if (bi->left)
        v_count += 3*4;
      if (bi->right)
        v_count += 3*4;
      if (bi->top)
        v_count += 3*4;
      if (bi->bottom)
        v_count += 3*4;
    }
  }

  PHYSFS_deinit();
  return 0;
}
