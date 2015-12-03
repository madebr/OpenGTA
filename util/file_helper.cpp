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
#include <cstdlib>
#include <physfs.h>
#include <sstream>
#include "m_exceptions.h"
#include "file_helper.h"
#include "buffercache.h"

#ifndef OGTA_DEFAULT_DATA_PATH
#define OGTA_DEFAULT_DATA_PATH "gtadata.zip"
#endif
#ifndef OGTA_DEFAULT_MOD_PATH
#define OGTA_DEFAULT_MOD_PATH ""
#endif

#ifndef OGTA_DEFAULT_HOME_PATH
//#ifdef LINUX
//#define OGTA_DEFAULT_HOME_PATH PHYSFS_getUserDir()
//#elif WIN32
//#define OGTA_DEFAULT_HOME_PATH "config" 
//#endif
#define OGTA_DEFAULT_HOME_PATH PHYSFS_getBaseDir()
#endif

namespace Util {
  FileHelper::FileHelper() :
    baseDataPath(OGTA_DEFAULT_DATA_PATH),
    modDataPath(OGTA_DEFAULT_MOD_PATH),
    userHomeDir(OGTA_DEFAULT_HOME_PATH) {
    const char *e = getenv("OGTA_DATA");
    if (e != NULL)
      baseDataPath = std::string(e);
    e = getenv("OGTA_MOD");
    if (e != NULL)
      modDataPath = std::string(e);
    e = getenv("OGTA_HOME");
    if (e != NULL)
      userHomeDir = std::string(e);
  }

  const std::string & FileHelper::getBaseDataPath() const {
    return baseDataPath;
  }

  const std::string & FileHelper::getModDataPath() const {
    return modDataPath;
  }

  const std::string & FileHelper::getUserHomeDir() const {
    return userHomeDir;
  }

  bool FileHelper::existsInSystemFS(const std::string & file) const {
    if (file.length() == 0)
      return 0;
#ifdef LINUX
    return (access(file.c_str(), R_OK) == 0);
#endif
#ifdef WIN32
    FILE * f = fopen(file.c_str(), "rb");
    bool res = (f != NULL);
    if (f)
      fclose(f);
    return res;
#endif
  }

  bool FileHelper::existsInVFS(const std::string & file) const {
    if (file.length() == 0)
      return 0;
    return PHYSFS_exists(file.c_str());
  }

  PHYSFS_file *FileHelper::openReadVFS(const std::string & file) const {
    PHYSFS_file * fd = PHYSFS_openRead(file.c_str());
    if (fd)
      return fd;
    // try lower case
    std::string name2(file);
    transform(name2.begin(), name2.end(), name2.begin(), tolower);
    fd = PHYSFS_openRead(name2.c_str());
    if (!fd) { // still no joy, give up
      std::ostringstream o;
      o << file << " with error: " << PHYSFS_getLastError();
      throw E_FILENOTFOUND(o.str());
    }
    // take this one instead
    return fd;
  }

  unsigned char* FileHelper::bufferFromVFS(PHYSFS_file *fd) const {
    assert(fd);
    unsigned int size = PHYSFS_fileLength(fd);
    unsigned char* buffer = BufferCacheHolder::Instance().requestBuffer(size+1);
    size = PHYSFS_read(fd, buffer, 1, size);
    PHYSFS_close(fd);
    return buffer; 
  }
}
