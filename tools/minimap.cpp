#include <cassert>
#include "opengta.h"
#include "log.h"

int main(int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr << "USAGE: minimap map_filename" << std::endl;
    std::cerr << "saves map as 'out.bmp'" << std::endl;
    return 1;
  }

  PHYSFS_init(argv[0]);
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
  PHYSFS_addToSearchPath("gtadata.zip", 1);

  std::string map_filename(argv[1]);
  OpenGTA::Map map(map_filename);

  uint32_t green = 0x00dd00ff;
  uint32_t red   = 0xdd0000ff;
  uint32_t blue  = 0x0000ddff;

  uint32_t field = green;
  uint32_t building = 0xdf9f6fff;
  uint32_t water = blue;
  uint32_t road = 0xdadadaff;
  uint32_t pavement = 0x8a9aa0ff;

  uint32_t map_color[] = {
    0x000000ff,
    water,
    road,
    pavement,
    field,
    building,
    0xffffffff,
    0xff0000ff
  };


  SDL_Init(SDL_INIT_VIDEO);
  
// FIXME: doesn't work right
#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define rmask 0xff000000 
#define gmask 0x00ff0000 
#define bmask 0x0000ff00 
#define amask 0x000000ff
#else 
#define rmask 0x000000ff 
#define gmask 0x0000ff00
#define bmask 0x00ff0000 
#define amask 0xff000000 
#endif 

  SDL_Surface* surface = SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 
      256, 256, 32,// rmask, gmask, bmask, amask);
  0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
  SDL_LockSurface(surface);
  uint32_t* dst = static_cast<uint32_t*>(surface->pixels);

  for (int i = 0; i < 256; i++) {
    for (int j = 0; j < 256; j++) {
      PHYSFS_uint16 emptycount = map.getNumBlocksAtNew(j,i);
      int found_type = 0;
      //for (int c=6-emptycount; c > 0; c--) {
      for (int c = 0; c < emptycount ; ++c) {
        OpenGTA::Map::BlockInfo* bi = map.getBlockAtNew(j, i, c);
        /*
        if (bi->blockType() > 0)
          found_type = bi->blockType();
        */

        /*
        if (bi->railway())
          found_type = 2;
        if (bi->railStation())
          found_type = 0;
        if (bi->railStationTrain())
          std::cout << " train: " << i << ", " << j << std::endl;
        if (bi->railStartTurn())
          std::cout << " start: " << i << ", " << j << std::endl;
        if (bi->railEndTurn())
          std::cout << " end: " << i << ", " << j << std::endl;
        */
        /*
        if (bi->blockType() > 0)
          found_type = bi->blockType();
        if (bi->trafficLights())
          found_type = 0;
        */
        if (bi->railway())
          found_type = 7;
      }
      if (found_type)
        *dst = map_color[found_type];
      else
        *dst = 0x000000ff;
      dst++;
    }
  }
  SDL_UnlockSurface(surface);
  SDL_SaveBMP(surface, "out.bmp");

  SDL_Quit();
  return 0;
}
