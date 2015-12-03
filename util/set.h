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
#ifndef SET_H
#define SET_H

namespace Util {

  /* Implements a set for non-negative integer values.
   * storage is a char* which is either a member of this class,
   * or given to an instance via alternative constructor
   * or the set_data() function.
   * Some macros are used to move to the correct char and
   * read/write the binary data.
   */
  class Set {
    public:
      /** Base constructor.
       * Creates a set of size #MAX_SET_COUNT.
       */
      Set();

      /** Constructor with defined size. 
       * @param n the element count in an integer variable
       */
      Set(int n);
      /** Copy constructor, copies internal data of another Set.
       * @param other reference to the other set
       *
       * \note This is a deep copy, rather than a shallow one; meaning
       * this set will allocate memory and memcpy the data, not
       * hold a pointer into the other set.
       */
      Set(const Set & other);

      /** Constructor with external data set.
       * @param n Size of storage (in bits)
       * @param data pointer to external storage
       * \note The set will not handle freeing of this memory,
       * it just works on it.
       */
      Set(int n, unsigned char* data);

      ~Set();

      /** Give another storage pointer to an instance.
       * @param n size
       * @param data pointer to data
       * @note This only works for instances that were already
       * constructed with external data.
       */
      void set_data(int n, unsigned char* data);

      /** Mutator for the last index.
       * @param n the element count
       * @note You should only shrink sets previously defined @e larger,
       * not the other way round!
       */
      void set_last(int n);

      /** Set2Number - least significant bits first.
       */
      int as_int(int start, int len) const;

      /** Set2Number - most significant bits first.
       */
      int as_int2(int start, int len) const;

      /** Accessor for the last index.
       * @return n the element count
       */
      int get_last() const;

      /** Mutator for the contained data.
       * @param k the index of the item to change
       * @param val true (k-th item in set) or false (not in set)
       */
      void set_item(int k, bool val);

      /** Accessor for the contained data.
       * @param k item index to query
       * @return val either true (item in set) or false (not in set)
       */
      bool get_item(int k) const;

      /** Test set for inclusion with another.
       * the return value is either:
       * 0  - the two sets are disjoint
       * 1  - this set contains the other
       * 2  - this set contains a subset of the other
       * -1 - the other set contains this
       * -2 - the other contains a subset of this
       * 3  - the two sets are equal
       * @param other a reference to another Set
       * @return res an int value as listed above
       */
      int compare(const Set & other) const;

      /** Returns the address of the internal storage data.
       * KNOW WHAT YOU ARE DOING WITH THIS
       * @return storage a char* to the entire storage block
       */
      unsigned char* give_storage() const;

      void print_set() const;

    protected:
      int last;
    private:
      unsigned char* storage;
      bool ext_data;
      // make sure this is a power of 8
      static const int MAX_SET_COUNT = 32;
  };
}
#endif
