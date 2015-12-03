#include "log.h"
#include <SDL_opengl.h>

namespace Util {
  unsigned int Log::level = 0;
  std::ostream Log::emptyStream(0);

  void Log::setOutputLevel(unsigned int newLevel) {
    level = newLevel;
  }

  const char* Log::glErrorName(int k) {
    switch(k) {
      case GL_NO_ERROR:
        return "GL_NO_ERROR";
      case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";
      case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";
      case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
      case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
      case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";
    }
    return "Unknown-Error";
  }
}
