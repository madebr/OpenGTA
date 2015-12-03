#ifndef MAP_HELPER_H
#define MAP_HELPER_H

#include <list>
#include <SDL_video.h>
#include "yasli/random.h"

namespace Util {
  typedef std::pair<SDL_Rect, SDL_Rect> TupleOfRects;
  typedef std::pair<uint8_t, uint8_t> TupleOfUint8;
  typedef std::list< TupleOfRects > ListOfTupleOfRects;

  class SpriteCreationArea {
    public:
      //ListOfTupleOfRects validTuple;
      TupleOfRects validRects;
      void setRects(const SDL_Rect & allowed, const SDL_Rect & denied);
      TupleOfUint8 getValidCoord();
      Random rnd;
  };
}
#endif
