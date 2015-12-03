#include <iostream>
#include "rectangle.hpp"

using namespace std;
using namespace Math;

template <typename T> bool test_values ( const Rectangle<T> & r, T x, T y, T w, T h ) {
  return (r.x == x && r.y == y && r.w == w && r.h == h);
}

int main(int argc, char* argv[]) {
/*
#define TEST_SET_1 5, 10, 24, 31
  Rectangle<int> a(TEST_SET_1);
  if (!test_values<int>(a, TEST_SET_1))
    cerr << "verifying values failed " << __LINE__ << endl;

  Rectangle<int> a_copy(a);
  if (!test_values<int>(a_copy, TEST_SET_1))
    cerr << "copy-constructor failed " << __LINE__ << endl;

  if (! (a == a_copy) )
    cerr << "equality failed " << __LINE__ << endl;

  if (! a.isInside(15, 15) )
    cerr << "isInside failed " << __LINE__ << endl;


  Rectangle<int> b(10, 15, 10, 15);
  if (a == b )
    cerr << "equality failed " << __LINE__ << endl;

  if (! rectangle_test_inside<int>(a, b) )
    cerr << "rect-inside-rect test failed " << __LINE__ << endl;

  if (rectangle_test_leftborder<int>(a, b))
    cerr << "rectangle-left-b failed " << __LINE__ << endl;

  if (rectangle_test_rightborder<int>(a, b))
    cerr << "rectangle-right-b failed " << __LINE__ << endl;

  if (rectangle_test_topborder<int>(a, b))
    cerr << "rectangle-top-b failed " << __LINE__ << endl;

  if (rectangle_test_bottomborder<int>(a, b))
    cerr << "rectangle-bottom-b failed " << __LINE__ << endl;

*/

  typedef Rectangle<int> IRect;

  int input[4];
  for (int j = 0; j < 4; ++j) {
    input[j] = 0;
    input[j] = atoi(argv[j+1]);
  }

  IRect big(input[0], input[1], input[2], input[3]);
  for (int j = 0; j < 4; ++j) {
    input[j] = 0;
    input[j] = atoi(argv[j+5]);
  }


  IRect small(input[0], input[1], input[2], input[3]);
  list< IRect > mlist;
  RectangleGeometry<int>::difference(mlist, big, small);

  cout << mlist.size() << " rects returned" << endl;
  list< IRect >::iterator i;
  for (i = mlist.begin(); i != mlist.end(); ++i) {
    cout << "x = " << i->x << " y = " << i->y
    << " w = " << i->w << " h = " << i->h << endl;
  }
}
