/* derived from:
 *
 * grand theft auto .sdt format 
 * copyright 2002 Delfi delfi@volja.net
 */

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
#ifndef OGTA_FX_SDT_H
#define OGTA_FX_SDT_H
#include <physfs.h>
#include <string>
#include <map>

namespace OpenGTA {
  class SoundsDB {
    public:
      SoundsDB();
      SoundsDB(const std::string & sdt_file);
      ~SoundsDB();
      void load(const std::string & sdt_file);
      struct Entry {
        Entry(PHYSFS_uint32, PHYSFS_uint32, PHYSFS_uint32);
        PHYSFS_uint32 rawStart;
        PHYSFS_uint32 rawSize;
        PHYSFS_uint32 sampleRate;
      };
      typedef PHYSFS_uint16 KeyType;
      Entry & getEntry(KeyType key);
      unsigned char* getBuffered(KeyType key);

    private:
      void clear();
      typedef std::map<KeyType, Entry> MapType;
      MapType knownEntries;
      PHYSFS_file *dataFile;
  };
}

#endif
