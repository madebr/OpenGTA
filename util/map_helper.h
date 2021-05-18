#ifndef MAP_HELPER_H
#define MAP_HELPER_H

#include <list>
#include <map>
#include <random>
#include <SDL_video.h>
#include "math3d.h"

namespace Util {
  typedef std::pair<SDL_Rect, SDL_Rect> TupleOfRects;
  typedef std::pair<uint8_t, uint8_t> TupleOfUint8;
  typedef std::list< TupleOfRects > ListOfTupleOfRects;

  class SpriteCreationArea {
    public:
      SpriteCreationArea();
      //ListOfTupleOfRects validTuple;
      TupleOfRects validRects;
      void setRects(const SDL_Rect & allowed, const SDL_Rect & denied);
      TupleOfUint8 getValidCoord();
      bool isOnScreen(const Vector3D & p);
      bool isOffScreen(const Vector3D & p);
    private:
      SDL_Rect onScreen;
      std::mt19937 rng_;
  };


  /** lesser than comparison of two std::pair<int, int>.
   */
  template <typename T>
    struct lt_pair {
      bool operator() (const T & a, const T & b) const {
        if (a.first < b.first)
          return true;
        if (a.first > b.first)
          return false;
        return (a.second < b.second);
      }
    };

  typedef std::pair<int, int> Pair2Int;
  typedef lt_pair<std::pair<int, int> > cmp_Pair2Int;
  typedef std::map<Pair2Int, int, cmp_Pair2Int> MapOfPair2Int;

  /** count of Pair2Int(a,b) in the map.
   * @return 0 if no entry at that \e position
   * @return count otherwise
   */
  int item_count(MapOfPair2Int & m, int a, int b);
  /** insert (may overwrite) count 1 at Pair2Int(a, b).
   */
  void register_item1(MapOfPair2Int & m, int a, int b);
  /** register and count Pair2Int(a, b).
   */
  void register_item(MapOfPair2Int & m, int a, int b);


}
#endif
