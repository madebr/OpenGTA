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
#ifndef UTIL_FILEHELPER_H
#define UTIL_FILEHELPER_H

#include <string>
#include <physfs.h>

namespace Util {
  // central path storage
  // Note: assumes that physfs is ready when instance is created
  class FileHelper {
    private:
      FileHelper();
      ~FileHelper() = default;
    public:
      FileHelper(const FileHelper& copy) = delete;
      FileHelper& operator=(const FileHelper& copy) = delete;

      static FileHelper& Instance()
      {
        static FileHelper instance;
        return instance;
      }

      const std::string & getBaseDataPath() const;
      const std::string & getModDataPath() const;
      const std::string & getUserHomeDir() const;
      bool existsInSystemFS(const std::string & file) const;
      bool existsInVFS(const std::string & file) const;
      PHYSFS_file* openReadVFS(const std::string & file) const;
      unsigned char* bufferFromVFS(PHYSFS_file*) const;
      static const std::string lang2MsgFilename(const char* l);
    private:
      std::string baseDataPath;
      std::string modDataPath;
      std::string userHomeDir;
  };
}
#define GET_FILE_HELPER Util::FileHelper::Instance()

#endif
