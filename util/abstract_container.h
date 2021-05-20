/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
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
#ifndef ABSTRACT_CONTAINER_H
#define ABSTRACT_CONTAINER_H

#include <string>
#include <list>
#include "m_exceptions.h"

template <class T>
class AbstractContainer {
  public:
    T &  doGet(uint32_t);
    void doRemove(uint32_t);
    inline T & doAdd(const T & t) {
      objs.push_back(t);
      return *objs.rbegin();
    }
    void doRealRemove();
  protected:
    typedef typename std::list<T> Storage_T;
    Storage_T objs;
    typedef typename std::list<T>::iterator Storage_T_Iterator;
    typedef typename std::list< Storage_T_Iterator > Iterator_Storage;
    Iterator_Storage toBeRemoved;
  private:
    Storage_T_Iterator findById(uint32_t & id);
};

template <class T>
void AbstractContainer<T>::doRealRemove() {
  typename Iterator_Storage::iterator i = toBeRemoved.begin();
  while (i != toBeRemoved.end()) {
    typename Iterator_Storage::iterator j = i++;
    typename Storage_T::iterator ii = *j;
    // remove the pointed-to object
    objs.erase(ii);
    // remove from list of lists
    toBeRemoved.erase(j);
  }
}

template <class T>
void AbstractContainer<T>::doRemove(uint32_t id) {
  typename Storage_T::iterator i = findById(id);
  toBeRemoved.push_back(i);
}

template <class T>
T & AbstractContainer<T>::doGet(uint32_t id) {
  typename Storage_T::iterator i = findById(id);
  return *i;
}

template <class T>
typename std::list<T>::iterator AbstractContainer<T>::findById(uint32_t & id) {
  typename Storage_T::iterator i;
  for (i = objs.begin(); i != objs.end(); ++i) {
    if (i->id() == id)
      return i;
  }
  throw E_UNKNOWNKEY(std::to_string(id));
}

#endif
