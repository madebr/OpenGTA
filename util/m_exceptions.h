#ifndef LOCAL_EXCEPTIONS_OH_WHAT_JOY
#define LOCAL_EXCEPTIONS_OH_WHAT_JOY

#include <exception>
#include <iostream>

namespace Util {

  struct LocalException : public std::exception {
    LocalException(const char *f, const size_t l, const char* n);
    LocalException(const char *f, const size_t l, const char* n, 
        const std::string _msg);
    virtual ~LocalException() throw() {}
    const char * what() const throw();
    std::string inFile;
    std::string typeName;
    std::string msg;
    size_t      inLine;
  };

  /* Actually I wouldn't make the derived destructors virtual,
   * but g++ complains if I don't. No idea why.
   *
   * Anyway exceptions shouldn't happen that often and gcc is
   * usually right about those things...
   */

  struct FileNotFound : public LocalException {
    FileNotFound(const char* f, const size_t l, const std::string _msg);
    virtual ~FileNotFound() throw() {}
  };

  struct IOError : public LocalException {
    IOError(const char* f, const size_t l, const std::string _msg);
    virtual ~IOError() throw() {}
  };

  struct InvalidFormat : public LocalException {
    InvalidFormat(const char* f, const size_t l, const std::string _msg);
    virtual ~InvalidFormat() throw() {}
  };

  struct UnknownKey : public LocalException {
    UnknownKey(const char* f, const size_t l, const std::string _msg);
    virtual ~UnknownKey() throw() {}
  };

  struct OutOfRange : public LocalException {
    OutOfRange(const char* f, const size_t l, const std::string _msg);
    virtual ~OutOfRange() throw () {}
  };

  struct OutOfMemory : public LocalException {
    OutOfMemory(const char* f, const size_t l, const std::string _msg);
    virtual ~OutOfMemory() throw() {}
  };

  struct ScriptError : public LocalException {
    ScriptError(const char* f, const size_t l, const std::string _msg);
    virtual ~ScriptError() throw() {}
  };

  struct NotSupported : public LocalException {
    NotSupported(const char* f, const size_t l, const std::string _msg);
    virtual ~NotSupported() throw() {}
  };
}

// to avoid the need for the namespace when writing catch-all blocks 
typedef Util::LocalException Exception;

#ifdef WIN32
#undef E_OUTOFMEMORY
#endif

// to auto-fill line+file information where the exception was created
#define E_FILENOTFOUND(m)  Util::FileNotFound(__FILE__, __LINE__, m)
#define E_IOERROR(m)       Util::IOError(__FILE__, __LINE__, m)
#define E_INVALIDFORMAT(m) Util::InvalidFormat(__FILE__, __LINE__, m)
#define E_UNKNOWNKEY(m)    Util::UnknownKey(__FILE__, __LINE__, m)
#define E_OUTOFRANGE(m)    Util::OutOfRange(__FILE__, __LINE__, m)
#define E_OUTOFMEMORY(m)   Util::OutOfMemory(__FILE__, __LINE__, m)
#define E_SCRIPTERROR(m)   Util::ScriptError(__FILE__, __LINE__, m)
#define E_NOTSUPPORTED(m)  Util::NotSupported(__FILE__, __LINE__, m)

#endif
