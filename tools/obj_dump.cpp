#include <cassert>
#include <iostream>
#include <string>
#include "opengta.h"

extern int global_EC;
extern int global_Done;
std::string map_file("undefined_map_file");
std::string style_file("undefined_style_file");

void on_exit() {
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
}

void parse_args(int argc, char* argv[]) {
#ifdef DUMP_OBJ_IN_MAP
  if (argc != 3) {
    std::cout << "USAGE: " << argv[0] << " map-file 8-bit-style-file" << std::endl;
    exit(1);
  }
  map_file = argv[1];
  style_file = argv[2];
#else
  if (argc != 2) {
    std::cout << "USAGE: " << argv[0] << " map-file 8-bit-style-file" << std::endl;
    exit(1);
  }
  map_file = "";
  style_file = argv[1];
#endif

}


void run_init() {
  PHYSFS_init("mapview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
}

#ifdef DUMP_OBJ_IN_MAP
void run_main() {
  OpenGTA::Map map(map_file);
  OpenGTA::Graphics8Bit style(style_file);

  for (int i = 0; i < map.numObjects; i++) {
    OpenGTA::Map::ObjectPosition & o = map.objects[i];
    std::cout << "object: " << i << std::endl;
    std::cout << o.x << ", " << o.y << ", " << o.z << " rot: " << o.rotation <<
      " pitch: " << o.pitch << " roll: " << o.roll << std::endl;
    std::cout << "type: " << int(o.type);
    if (o.remap >= 128) {
      std::cout << " (a car) remap: " << int(o.remap - 128) << std::endl;
      OpenGTA::GraphicsBase::CarInfo * info = style.findCarByModel(o.type);
      assert(info);
      std::cout << "width: " << info->width << " height: "<< info->height <<
      " depth " << info->depth << " sprnum: " << info->sprNum << " weight: " << info->weightDescriptor
      << std::endl;
    }
    else {
      std::cout << " (an obj) remap: " << int(o.remap) << std::endl;
      OpenGTA::GraphicsBase::ObjectInfo * info = style.objectInfos[o.type];
      std::cout << "width: " << info->width << " height: "<< info->height <<
      " depth " << info->depth << " sprnum: " << info->sprNum << " weight: " << info->weight
      << " aux: " << info->aux << " status: " << int(info->status) << " n-into: " << int(info->numInto) << std::endl;
    }
    std::cout << std::endl;
  }
}

#else
// dump object/sprite-infos
void run_main() {
  OpenGTA::Graphics8Bit style(style_file);
  std::cout << "DUMP_OBJ_INFO BEGIN" << std::endl;
  for (size_t i = 0; i < style.objectInfos.size(); ++i) {
    std::cout << "obj-type: " << i << "  width: " << style.objectInfos[i]->width << " height: " 
    << style.objectInfos[i]->height << " depth: " << style.objectInfos[i]->depth << 
    " spr-num: " << style.objectInfos[i]->sprNum <<  " reindex: " << style.spriteNumbers.reIndex(style.objectInfos[i]->sprNum, OpenGTA::GraphicsBase::SpriteNumbers::OBJECT) << " weight: " << style.objectInfos[i]->weight << " aux: " <<
    style.objectInfos[i]->aux << " status: " << int(style.objectInfos[i]->status) << " num-into: " << 
    int(style.objectInfos[i]->numInto) << std::endl;
  }
  std::cout << "DUMP_OBJ_INFO END" << std::endl;
  for (size_t i = 0; i < style.spriteInfos.size(); ++i) {
    std::cout << "sprite: " << i << "  width: " << int(style.spriteInfos[i]->w) << " height: " <<
      int(style.spriteInfos[i]->h) << " delta-count: " << int(style.spriteInfos[i]->deltaCount)<< std::endl;
  }

}

#endif
