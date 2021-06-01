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

#define ANSI_COLOR_OFF "\033[0m"
#define ANSI_COLOR_INFO "\033[32m"
#define ANSI_COLOR_WARN "\033[33m"
#define ANSI_COLOR_ERR  "\033[31m"

namespace Util {
  class Log {
    public:
      inline static std::ostream & info(const char* f, int l) {
        if (level) return emptyStream;
        std::cout
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_INFO
#endif
        << "Info (" << f << ":" << l << "): "
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_OFF
#endif
        ;
        return std::cout;
      }
      inline static std::ostream & warn(const char* f, int l) {
        if (level > 1) return emptyStream;
        std::cerr
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_WARN
#endif
        << "Warning (" << f << ":" << l << "): "
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_OFF
#endif
        ;
        return std::cerr;
      }
      inline static std::ostream & error(const char* f, int l) {
        std::cerr
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_ERR
#endif
        << "Error (" << f << ":" << l << "): "
#ifdef LOG_USE_ANSI_COLORS
        << ANSI_COLOR_OFF
#endif
        ;
        return std::cerr;
      }
      static void setOutputLevel(unsigned int newLevel);
      static const char* glErrorName(int k);
    private:
      static unsigned int level;
      static std::ostream emptyStream;
  };
}
#endif
