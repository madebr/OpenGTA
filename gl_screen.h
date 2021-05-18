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
#ifndef GL_SCREEN_H
#define GL_SCREEN_H

#include <SDL.h>
#include <SDL_opengl.h>


namespace OpenGL {
  class Screen {
    private:
      Screen();
      ~Screen();
    public:
      Screen(const Screen& copy) = delete;
      Screen& operator=(const Screen& copy) = delete;

      static Screen& Instance()
      {
        static Screen instance;
        return instance;
      }

      void set3DProjection();
      void setFlatProjection();
      void setFullScreenFlag(bool v);
      void toggleFullscreen();
      void activate(Uint32 w = 0, Uint32 h = 0);
      void resize(Uint32 w, Uint32 h);
      void setSystemMouseCursor(bool visible);
      Uint32  getWidth();
      Uint32  getHeight();
      bool getFullscreen();
      void makeScreenshot(const char* filename);
      inline float getFieldOfView() { return fieldOfView; }
      inline float getNearPlane() { return nearPlane; }
      inline float getFarPlane() { return farPlane; }
      void setupGlVars( float fov, float near_p, float far_p);
      void setupVsync( size_t enabled );

      static GLboolean queryExtension(const char *extName);

    private:
      void initGL();
      void initSDL();
      Uint32 width, height;
      Uint32 bpp;
      Uint32 videoFlags;
      size_t useVsync;
      float  fieldOfView;
      float  nearPlane;
      float  farPlane;
      static const Uint32 defaultVideoFlags = 
        SDL_OPENGL | SDL_GL_DOUBLEBUFFER;// | SDL_HWPALETTE | SDL_HWACCEL;
      //FIXME: use   ^ here as well? not just SDL_GL_SetAttribute?

      SDL_Surface *surface;
  };
}

#endif
