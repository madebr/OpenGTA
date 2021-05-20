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
#include <iostream>
#include <string>
#include <cassert>
#include <cstring>
#include "buffercache.h"
#include "log.h"
#include "m_exceptions.h"

namespace Util {

  BufferCache::BufferCache() {
    total_bytes = 0;
  }
  BufferCache::~BufferCache() {
    BufferMap_t::const_iterator i = allocated.begin();
    while (i != allocated.end()) {
      freeBuffer(i);
      i++;
    }
    allocated.clear();
    BufferMap_T::const_iterator j = locked.begin();
    if (locked.size())
      WARN << "Locked elements remaining in cache: " << locked.size() << std::endl;
    while (j != locked.end()) {
      delete [] j->first;
      j++;
    }
  }

  unsigned char* BufferCache::requestBuffer(unsigned int len) {
    unsigned char *result = NULL;
    BufferMap_t::const_iterator i = findMatchingBuffer(len);
    if (i == allocated.end()) {
      INFO << "No matching buffer for size: " << len << std::endl;
      result = createBuffer(len);
      if (result == NULL)
        throw E_OUTOFMEMORY("Failed to allocate buffer");
        //throw std::string("Error: out of memory");
      allocated[len] =result;
      printStatus();
    }
    else {
      result = i->second;
    }
    memset(result, 0, len);
    return result;
  }

  unsigned char* BufferCache::requestLockedBuffer(unsigned int len) {
    unsigned char *result = requestBuffer(len);
    lockBuffer(result);
    return result;
  }

  void BufferCache::lockBuffer(unsigned char* tb) {
    BufferMap_t::const_iterator i = allocated.begin();
    while (i != allocated.end()) {
      if (i->second == tb) {
        locked[i->second] = i->first;
        allocated.erase(i->first);
        return;
      }
      ++i;
    }
    throw E_UNKNOWNKEY(std::string { "Cannot lock unknown buffer " }
                       + reinterpret_cast<char *>(tb));
    //throw std::string("Unknown buffer - cannot lock it");
  }

  void BufferCache::unlockBuffer(unsigned char* tb) {
    BufferMap_T::const_iterator i = locked.find(tb);
    if (i == locked.end()) {
      throw E_UNKNOWNKEY(std::string { "Cannot unlock unknown buffer " }
                         + reinterpret_cast<char *>(tb));
      //throw std::string("Unknow buffer - cannot unlock it");
    }
    allocated[i->second] = i->first;
    locked.erase(i->first);
  }

  BufferCache::BufferMap_t::const_iterator BufferCache::findMatchingBuffer(unsigned int len) {
    BufferMap_t::const_iterator i = allocated.find(len);
    if (i != allocated.end())
      return i;
    i = allocated.begin();
    while (i != allocated.end()) {
      if (i->first > len)
        return i;
      ++i;
    }
    return allocated.end();
  }

  void BufferCache::printStatus() {
    INFO << "status: " << allocated.size() << " allocated " <<
      locked.size() << " locked buffers " << total_bytes <<
      " bytes" << std::endl;
  }

  unsigned char* BufferCache::createBuffer(unsigned int len) {
    total_bytes += len;
    return new unsigned char[len];
  }

  void BufferCache::freeBuffer(BufferMap_t::const_iterator pos) {
    delete [] pos->second;
    total_bytes -= pos->first;
  }

  BufferCache::LockedBuffer::LockedBuffer(unsigned char *b) {
    assert(b);
    buf_p = b;
  }

  BufferCache::LockedBuffer::LockedBuffer(unsigned int len) {
    buf_p = BufferCache::Instance().requestLockedBuffer(len);
  }

  BufferCache::LockedBuffer::~LockedBuffer() {
    BufferCache::Instance().unlockBuffer(buf_p);
  }

}
