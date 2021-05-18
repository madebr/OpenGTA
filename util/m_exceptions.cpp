#include "m_exceptions.h"
#include <sstream>

namespace Util {

  LocalException::LocalException(const char *f, const size_t l, const char *n)
      : msg { std::string { n } + " (" + f + ":" + std::to_string(l) }
  {}

  LocalException::LocalException(const char *f,
                                 const size_t l,
                                 const char *n,
                                 const std::string _msg)
      : msg { std::string { n } + " (" + f + ":" + std::to_string(l)
              + "): " + msg }
  {}

  FileNotFound::FileNotFound(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "FileNotFound", _msg) {}

  IOError::IOError(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "IOError", _msg) {}

  InvalidFormat::InvalidFormat(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "InvalidFormat", _msg) {}

  UnknownKey::UnknownKey(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "UnknownKey", _msg) {}

  OutOfRange::OutOfRange(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "OutOfRange", _msg) {}

  OutOfMemory::OutOfMemory(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "OutOfMemory", _msg) {}

  ScriptError::ScriptError(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "ScriptError", _msg) {}

  NotSupported::NotSupported(const char* f, const size_t l, const std::string _msg) :
    LocalException(f, l, "NotSupported", _msg) {}

}

#if 0
using namespace Util;
int main(int argc, char* argv[]) {
  try {
    throw E_FILENOTFOUND("foobar.zip");
  }
  catch (Exception & e) {
    std::cout << "E: " << e.what() << std::endl;
  }
}
#endif
