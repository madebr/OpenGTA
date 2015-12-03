#include "map_helper.h"
#include "log.h"

namespace Util {
  void SpriteCreationArea::setRects(const SDL_Rect & allowed, const SDL_Rect & denied) {
    validRects = std::make_pair<SDL_Rect, SDL_Rect>(allowed, denied);
  }

  TupleOfUint8 SpriteCreationArea::getValidCoord() {
    uint32_t x = rnd.nextUint(validRects.first.w) + validRects.first.x;
    uint32_t y = rnd.nextUint(validRects.first.h) + validRects.first.y;
    return std::make_pair<uint8_t, uint8_t>(x, y);
  }
}
