#ifndef LOG_FUNCS_H
#define LOG_FUNCS_H
#include <iostream>

#ifdef WIN32
#undef ERROR
#endif

#define INFO  Util::Log::info(__FILE__, __LINE__)
#define WARN  Util::Log::warn(__FILE__, __LINE__)
#define ERROR Util::Log::error(__FILE__, __LINE__)
#define ERROR_AND_EXIT(ec) error_code = ec; exit(ec);
#define GL_CHECKERROR { int _err = glGetError(); if (_err != GL_NO_ERROR) \
Util::Log::error(__FILE__, __LINE__) << "GL error: " << _err << " = " << Util::Log::glErrorName(_err) << std::endl; }
namespace Util {
  class Log {
    public:
      inline static std::ostream & info(const char* f, int l) { 
        if (level) return emptyStream; std::cout << "Info (" << f << ":" << l << "): "; return std::cout;
      }
      inline static std::ostream & warn(const char* f, int l) {
        if (level > 1) return emptyStream; std::cerr << "Warning (" << f << ":" << l << "): "; return std::cerr;
      }
      inline static std::ostream & error(const char* f, int l) {
        std::cerr << "Error ("<< f << ":" << l << "): "; return std::cerr;
      }
      static void setOutputLevel(unsigned int newLevel);
      static const char* glErrorName(int k);
    private:
      static unsigned int level;
      static std::ostream emptyStream;
  };
}
#endif
