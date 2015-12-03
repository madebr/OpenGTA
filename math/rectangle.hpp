#ifndef RECTANGLE_2D_H
#define RECTANGLE_2D_H

#include <list>

namespace Math {

  template <typename U> struct Rectangle {
    U x;
    U y;
    U w;
    U h;

    Rectangle() : x(0), y(0), w(0), h(0) {}

    Rectangle(U _x, U _y, U _w, U _h) :
      x(_x), y(_y), w(_w), h(_h) {}

    Rectangle(const Rectangle<U> & o) :
      x(o.x), y(o.y), w(o.w), h(o.h) {}

    inline bool operator == (const Rectangle<U> & o) const {
      return ((x == o.x) && (y == o.y) && (w == o.w) && (h == o.h));
    }

    inline bool isInside(U _x, U _y) const {
      return ((_x >= x) && (_y >= y) &&
          (_x <= x + w) && (_y <= y + h) );
    }

  };

  template <typename U> bool rectangle_test_inside (
      const Rectangle<U> & larger, 
      const Rectangle<U> & smaller ) {
    return (larger.isInside( smaller.x, smaller.y ) &&    
        larger.isInside( smaller.x + smaller.w, smaller.y ) &&
        larger.isInside( smaller.x, smaller.y + smaller.h ) &&
        larger.isInside( smaller.x + smaller.w, smaller.y + smaller.h));
  }

  template <typename U> bool rectangle_test_leftborder (
      const Rectangle<U> & larger, 
      const Rectangle<U> & smaller ) {
    return (larger.x == smaller.x);
  }

  template <typename U> bool rectangle_test_rightborder (
      const Rectangle<U> & larger,
      const Rectangle<U> & smaller ) {
    return (larger.x + larger.w == smaller.x + smaller.w);
  }

  template <typename U> bool rectangle_test_bottomborder (
      const Rectangle<U> & larger, 
      const Rectangle<U> & smaller ) {
    return (larger.y == smaller.y);
  }

  template <typename U> bool rectangle_test_topborder (
      const Rectangle<U> & larger,
      const Rectangle<U> & smaller ) {
    return (larger.y + larger.h == smaller.y + smaller.h);
  }


  template <typename U> class RectangleGeometry {
    public:
      typedef std::list< Rectangle < U > > ListOfRectangleU;
      /*    void Union (ListOfRectangleU & list, const Rectangle< U > & first, 
            const Rectangle< U > & second);
            void Intersection(ListOfRectangleU & list, const Rectangle< U > & first, 
            const Rectangle< U > & second);
            */
      static void difference(ListOfRectangleU & list, const Rectangle< U > & first, 
          const Rectangle< U > & second);
  };

  template <typename U> void RectangleGeometry<U>::difference(ListOfRectangleU & list,
      const Rectangle< U > & first, const Rectangle< U > & second) {
    // assume first is larger, second is smaller
    if (! rectangle_test_leftborder<U>(first, second)) {
      /*
         ###                           #
         #x#    split off the left col #
         ###                           #
       */
      list.push_back( Rectangle<U>( first.x, first.y, second.x - first.x, first.h ) );
    }
    if (! rectangle_test_rightborder<U>(first, second)) {
      // split of the right column
      list.push_back( Rectangle<U>( second.x + second.w, first.y, 
        first.x + first.w - (second.x + second.w), first.h));
    }
    if (! rectangle_test_topborder<U>(first, second)) {
      // remainder of the top row
      list.push_back( Rectangle<U>( second.x, second.y + second.h,
        second.w, first.y + first.h - (second.y + second.h)));
    }
    if (! rectangle_test_bottomborder<U>(first, second)) {
      // remainder of the bottom row
      list.push_back( Rectangle<U>(second.x, first.y,
        second.w, second.y - first.y));
    }
  }
}

#endif
