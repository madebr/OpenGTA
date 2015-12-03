#ifndef UTIL_FILEHELPER_H
#define UTIL_FILEHELPER_H

#include <string>
#include <physfs.h>
#include "Singleton.h"

namespace Util {
  // central path storage
  // Note: assumes that physfs is ready when instance is created
  class FileHelper {
    public:
      FileHelper();
      const std::string & getBaseDataPath() const;
      const std::string & getModDataPath() const;
      const std::string & getUserHomeDir() const;
      bool existsInSystemFS(const std::string & file) const;
      bool existsInVFS(const std::string & file) const;
      PHYSFS_file* openReadVFS(const std::string & file) const;
    private:
      std::string baseDataPath;
      std::string modDataPath;
      std::string userHomeDir;
  };

  typedef Loki::SingletonHolder<FileHelper, Loki::CreateUsingNew, Loki::DefaultLifetime,
          Loki::SingleThreaded> FileHelperHolder;
}
#define GET_FILE_HELPER Util::FileHelperHolder::Instance()

#endif
