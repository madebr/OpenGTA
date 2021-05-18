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
#ifndef BUFFER_CACHE_H
#define BUFFER_CACHE_H
#include <map>

namespace Util {

  /** A simple memory-buffer pool.
   *
   * - You should never manually delete a buffer created through the cache.
   * - You should however \e unlock buffers that were locked once you
   * no longer need them.
   * - You don't need to check for NULL pointers; exceptions are used for
   * errors.
   * - Buffers are initialised with zeroes.
   *
   * Beware:
   * \code
   * // assuming the cache is empty
   *
   * a = requestBuffer(10)
   * b = requestBuffer(5)
   *
   * a == b
   * ----------------------
   * a = requestBuffer(5)
   * b = requestBuffer(6)
   *
   * a != b
   * \endcode
   */
  class BufferCache {
    private:
      BufferCache();
      ~BufferCache();
    public:
      BufferCache(const BufferCache& copy) = delete;
      BufferCache& operator=(const BufferCache& copy) = delete;

      static BufferCache& Instance()
      {
        static BufferCache instance;
        return instance;
      }

      /** Returns a buffer of (at least) len bytes.
       *
       * @param Length of requested buffer in bytes.
       * @return The buffer.
       * @note May throw a OutOfMemory exception.
       */
      unsigned char* requestBuffer(unsigned int len);
      /** Returns a locked buffer of (at least) len bytes.
       * Just a convenience function.
       *
       * @see requestBuffer
       * @see lockBuffer
       */
      unsigned char* requestLockedBuffer(unsigned int len);
      /** Lock a buffer (which was allocated by this cache).
       *
       * @param Pointer to the buffer.
       *
       * You need this when you request multiple buffers at the same time; lock
       * the current buffer before requesting the next one.
       */
      void lockBuffer(unsigned char* tb);
      /** Unlock a buffer (which was previously locked).
       * @param Pointer to the buffer.
       *
       * Returns the buffer to the shared pool.
       */
      void unlockBuffer(unsigned char* tb);
      class LockedBuffer {
        public:
          LockedBuffer(unsigned char* b);
          LockedBuffer(unsigned int len);
          ~LockedBuffer();
          inline unsigned char* operator()() { return buf_p; }
        private:
          unsigned char* buf_p;
      };
      void printStatus();
    private:
      typedef std::map<unsigned int, unsigned char*> BufferMap_t;
      typedef std::map<unsigned char*, unsigned int> BufferMap_T;
      BufferMap_t allocated;
      BufferMap_T locked;
      BufferMap_t::const_iterator findMatchingBuffer(unsigned int len);
      BufferMap_t::iterator       findLockedBuffer(unsigned char* tb);
      unsigned char* createBuffer(unsigned int len);
      void freeBuffer(BufferMap_t::const_iterator pos);
      unsigned int total_bytes;
  };
}
#endif
