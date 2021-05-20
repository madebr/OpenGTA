/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
*                                                                       *
* This software is provided as-is, without any express or implied       *
* warranty. In no event will the authors be held liable for any         *
* damages arising from the use of this software.                        *
*                                                                       *
* Permission is granted to anyone to use this software for any purpose, *
* including commercial applications, and to alter it and redistribute   *
* it freely, subject to the following restrictions:                     *
*                                                                       *
* 1. The origin of this software must not be misrepresented; you must   *
* not claim that you wrote the original software. If you use this       *
* software in a product, an acknowledgment in the product documentation *
* would be appreciated but is not required.                             *
*                                                                       *
* 2. Altered source versions must be plainly marked as such, and must   *
* not be misrepresented as being the original software.                 *
*                                                                       *
* 3. This notice may not be removed or altered from any source          *
* distribution.                                                         *
************************************************************************/
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <cstdio>
#include "set.h"

#define INTEGRATE_OGTA
#ifdef INTEGRATE_OGTA
#include "m_exceptions.h"
#endif

namespace Util {

  // some bits of binary magic from someone on IRC
  // by the name of Zhivago 
  // only the 8-bit variants are used
#define maskn(x, w)    ((x) & ((1<<w)-1))

#define tstbn(a, x, w) ((a)[x>>w] &   (1<<(maskn(x, w))))
#define setbn(a, x, w) ((a)[x>>w] |=  (1<<(maskn(x, w))))
#define clrbn(a, x, w) ((a)[x>>w] &= ~(1<<(maskn(x, w))))

#define tstb8(a, x) tstbn((a), (x), 3)
#define setb8(a, x) setbn((a), (x), 3)
#define clrb8(a, x) clrbn((a), (x), 3)

#define tstb32(a, x) tstbn((a), (x), 5)
#define setb32(a, x) setbn((a), (x), 5)
#define clrb32(a, x) clrbn((a), (x), 5)

  // and something to move through an array to the correct byte
#define mv2byte(a, k, p)  \
  if (k >= 8) { \
    p = a + k / 8; \
    k = k % 8; \
  } \
  else { \
    p = a; \
  } 

  // note: given a set of 1..N values
  // this means one needs to malloc(N/8 + (n%8 == 0 ? 0 : 1) bytes of mem
  // the bit status (reading from left to right) corresponds
  // to the existence of the index number in the set

  Set::Set() {
    last = MAX_SET_COUNT;
    storage = new unsigned char[MAX_SET_COUNT / 8];
    memset((void*)storage, 0, (size_t) MAX_SET_COUNT / 8);
    ext_data = 0;
  }

  Set::Set(int n) {
    last = n;
    assert(n>0);
    int k = ((n % 8) > 0) ? n / 8 + 1 : n / 8;
    storage = new unsigned char[k];
    memset((void*)storage, 0, (size_t)k);
    ext_data = 0;
  }

  Set::Set(const Set & other) {
    last = other.get_last();
    unsigned char* os = other.give_storage();
    assert(last > 0);
    int k = ((last % 8) > 0) ? last / 8 + 1 : last / 8;
    storage = new unsigned char[k];
    memcpy(storage, os, k);
    ext_data = 0;
  }

  Set::Set(int k, unsigned char* data) {
    storage = data;
    last = k;
    ext_data = 1;
  }

  Set::~Set() {
    if (!ext_data && storage != NULL)
      delete [] storage;
  }

  void Set::set_data(int n, unsigned char* data) {
    if (!ext_data) {
#ifdef INTEGRATE_OGTA
      throw E_NOTSUPPORTED("set_data() called on an instance with own data");
#else
      fprintf(stderr, "Set::Err: set_data() called on an instance with own data\n");
#endif
      return;
    }
  }

  void Set::set_last(int n) {
    if (n > last) {
#ifdef INTEGRATE_OGTA
      throw E_OUTOFRANGE(std::to_string(n) + " > " + std::to_string(last));
#else
      printf("%i is larger than previous last n (%i), aborting\n", n, last);
#endif
    }
    last = n;
  }

  int Set::get_last() const {
    return(last);
  }

  void Set::print_set() const {
    bool first = true;
    printf("{");
    for (int i = 0; i < last; i++) {
      if (get_item(i)) {
        if (!first) printf(", ");
        printf("%d", i);
        first = false;
      }
    }
    printf("}\n");
  }

  void Set::set_item(int k, bool val) {
    if (k < last) {
      unsigned char* pos = NULL;
      mv2byte(storage, k, pos);
      if (val == true) {
        setb8(pos, k);
      }
      else {
        clrb8(pos, k);
      }
    }
    else
#ifdef INTEGRATE_OGTA
    {
      throw E_OUTOFRANGE(std::to_string(k) + " >= " + std::to_string(last));
    }
#else
    assert(k < last);
#endif
  }

  bool Set::get_item(int k) const {
    bool b = false;
    if (k < last) {
      unsigned char* pos;
      mv2byte(storage, k, pos);
      b = tstb8(pos, k);
    }
    else
#ifdef INTEGRATE_OGTA
    {
      throw E_OUTOFRANGE(std::to_string(k) + " >= " + std::to_string(last));
    }
#else
    assert(k < last);
#endif
    return b;
  }

  int Set::as_int(int start, int len) const {
    if (start < 0 || start > last || start + len > last) {
#ifdef INTEGRATE_OGTA
      throw E_OUTOFRANGE("invalid query: " + std::to_string(start) + " length "
                         + std::to_string(len) + " with data-length "
                         + std::to_string(last));
#else
      fprintf(stderr, "Set::Err: queried index out of range (%i, %i ; %i)\n", start, len, last);
      return -1;
#endif
    }
    int v = 0;
    int t = 0;
    for (int i=0; i< len; i++) {
      if (get_item(i+start)) {
        v = (int) 1 << i;
        t += v;
      }
    }
    return t;
  }

  int Set::as_int2(int start, int len) const {
    if (start < 0 || start > last || start + len > last) {
#ifdef INTEGRATE_OGTA
      throw E_OUTOFRANGE("invalid query: " + std::to_string(start) + " length "
                         + std::to_string(len) + " with data-length "
                         + std::to_string(last));
#else
      fprintf(stderr, "Set::Err: queried index out of range (%i, %i ; %i)\n", start, len, last);
      return -1;
#endif
    }
    int v = 0;
    int t = 0;
    for (int i=0; i< len; i++) {
      if (get_item(i+start)) {
        v = (int)1 << (len-i-1);
        t += v;
      }
    }
    return t;
  }

  int Set::compare(const Set & other) const {
    int res = 0;
    unsigned char* oc;
    int o_s = 0;
    int l;
    int both = 0; // #elements in both sets
    int io = 0; // #in other set
    int it = 0; // #in this set

    o_s = other.get_last();
    oc = other.give_storage();
    // find the smaller index
    l = (last >= o_s)? o_s : last;
    // count away ;-)
    for (int i = 0; i < l; i++) {
      unsigned char* o;
      unsigned char* s;
      int j = i;
      mv2byte(storage, j, s);
      j = i;
      mv2byte(oc, j, o);
      if (tstb8(s, j)) {
        it++;
        if (tstb8(o, j)) {
          both++;
          io++;
        }
      }
      else {
        if (tstb8(o, j)) {
          io++;
        }
      }
    } // for
    //printf("both %i, it %i, io %i\n", both, it, io);
    if (both >= 0) { 
      if (both == it && both == io) {
        res = 3; // sets are equal (maybe empty)
      }
      else if (both == it) {
        res = -1; // this set is smaller than the other, other contains this
      }
      else if (both == io) {
        res = 1; // this set is larger and contains the other
      }
      else if (both > 0) {
        if (io > it) {
          res = -2;
        }
        else {
          res = 2;
        }
      }
      else res = 0;
    }
    return(res);
  }

  unsigned char* Set::give_storage() const {
    return(storage);
  }

}
