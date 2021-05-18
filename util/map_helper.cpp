#include "map_helper.h"
#include "log.h"

#include <chrono>

namespace {

std::mt19937::result_type get_seed()
{
  const auto secs = std::chrono::duration_cast<std::chrono::seconds>(
    std::chrono::system_clock::now().time_since_epoch()
  ).count();
  const auto msecs = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::high_resolution_clock::now().time_since_epoch()
  ).count();
  return std::mt19937::result_type(secs + msecs);
}

}

namespace Util {
  SpriteCreationArea::SpriteCreationArea()
    : rng_ { std::random_device {}() ^ get_seed() }
  {
  }

  void SpriteCreationArea::setRects(const SDL_Rect & allowed, const SDL_Rect & denied) {
    validRects = std::make_pair(allowed, denied);
    onScreen = denied;
  }

  bool SpriteCreationArea::isOnScreen(const Vector3D & p) {
/*    INFO << p.x << " " << p.y << " " << p.z << std::endl;
    INFO << onScreen.x <<", " << onScreen.y << " -> " <<
      onScreen.x + onScreen.w << ", " << onScreen.y + onScreen.h << std::endl;
*/
    if ((p.x >= onScreen.x) && (p.x <= onScreen.x + onScreen.w) &&
      (p.z >= onScreen.y) && (p.z <= onScreen.y + onScreen.h))
      return true;
    return false;
  }

  bool SpriteCreationArea::isOffScreen(const Vector3D & p) {
    return false;
  }

  TupleOfUint8 SpriteCreationArea::getValidCoord() {
    std::uniform_int_distribution<unsigned> distrW { 0, validRects.first.w };
    std::uniform_int_distribution<unsigned> distrH { 0, validRects.first.h };
    uint32_t x = distrW(rng_) + validRects.first.x;
    uint32_t y = distrH(rng_) + validRects.first.y;
    return std::make_pair(x, y);
  }

  int item_count(MapOfPair2Int & m, int a, int b) {
    MapOfPair2Int::iterator j = m.find( std::make_pair(a, b) );
    if (j == m.end())
      return 0;
    return j->second;
  }

  void register_item1(MapOfPair2Int & m, int a, int b) {
    m.insert( std::make_pair< Pair2Int, int>(
          std::make_pair(a, b),
          1)
        );
  }

  void register_item(MapOfPair2Int & m, int a, int b) {
    Pair2Int _p(std::make_pair(a, b));
    MapOfPair2Int::iterator j = m.find(_p);
    if (j == m.end())
      register_item1(m, a, b);
    else
      j->second++;
  }


}
