/************************************************************************
* Copyright (c) 2005-2006 tok@openlinux.org.uk                          *
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
#include <string>
#include "gl_screen.h"
#include "log.h"
#include "buffercache.h"
#include "m_exceptions.h"

namespace OpenGL {
#ifndef DEFAULT_SCREEN_WIDTH
#define DEFAULT_SCREEN_WIDTH 640
#endif
#ifndef DEFAULT_SCREEN_HEIGHT
#define DEFAULT_SCREEN_HEIGHT 480
#endif

  Screen::Screen() {
    surface = NULL;
    videoFlags = defaultVideoFlags; 
    width = DEFAULT_SCREEN_WIDTH;
    height = DEFAULT_SCREEN_HEIGHT;
    bpp = 32;
    fieldOfView = 60.0f;
    nearPlane = 0.1f;
    farPlane = 250.0f;
  }

  void Screen::activate(Uint32 w, Uint32 h) {
    if (w)
      width = w;
    if (h)
      height = h;
    initSDL();
    resize(width, height);
    initGL();
    setSystemMouseCursor(false);
  }

  void Screen::setSystemMouseCursor(bool visible) {
    SDL_ShowCursor((visible ? SDL_ENABLE : SDL_DISABLE));
  }

  Uint32 Screen::getWidth() {
    return width;
  }

  Uint32 Screen::getHeight() {
    return height;
  }

  bool Screen::getFullscreen() {
    return (videoFlags & SDL_FULLSCREEN);
  }

  void Screen::setFullScreenFlag(bool v) {
    if (v && getFullscreen())
      return;
    else if (!v && !getFullscreen())
      return;
    if (v)
      videoFlags |= SDL_FULLSCREEN;
    else
      videoFlags ^= SDL_FULLSCREEN;
  }

  Screen::~Screen() {
    setSystemMouseCursor(true);
    if (SDL_WasInit(SDL_INIT_VIDEO))
      SDL_Quit();
    surface = NULL;
  }

  void Screen::toggleFullscreen() {
    if (videoFlags & SDL_FULLSCREEN)
      videoFlags ^= SDL_FULLSCREEN;
    else
      videoFlags |= SDL_FULLSCREEN;
    resize(width, height);
  }

  void Screen::initSDL() {
    int err = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    if (err)
      //throw "SDL_Init failed: " + std::string(SDL_GetError());
      throw E_INVALIDFORMAT("SDL_Init failed: " + std::string(SDL_GetError()));
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1);
  }

  void Screen::initGL() {
    //GLfloat LightAmbient[]  = { 0.8f, 0.8f, 0.8f, 1.0f };
    //GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    //GLfloat LightPosition[] = { 128.0f, 200.0f, 128.0f, 1.0f };

    //glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable( GL_DEPTH_TEST );
    //glEnable( GL_LIGHTING );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    //glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
    //glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
    //glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
    //glEnable( GL_LIGHT0 );
    glEnable( GL_COLOR_MATERIAL);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_BACK, GL_LINE);
  }

  void Screen::resize(Uint32 w, Uint32 h) {
    if (h == 0)
      h = 1;
    surface = SDL_SetVideoMode(w, h, bpp, videoFlags);
    glViewport(0, 0, w, h);
    width = w;
    height = h;
  }

  void Screen::set3DProjection() {
    float ratio = float(width) / float(height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective( fieldOfView, ratio, nearPlane, farPlane);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void Screen::setFlatProjection() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, 0, height, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void Screen::makeScreenshot(const char* filename) {
    INFO << "saving screen as: " << filename << std::endl;
    uint8_t *pixels = Util::BufferCacheHolder::Instance().requestBuffer(width * height * 3);

    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, reinterpret_cast<GLvoid*>(pixels));

    SDL_Surface* image = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24,
        255U << (0),
        255U << (8),
        255U << (16),
        0);
    SDL_LockSurface(image);

    uint8_t *imagepixels = reinterpret_cast<uint8_t*>(image->pixels);
    for (int y = (height - 1); y >= 0; --y) {
      uint8_t *row_begin = pixels + y * width * 3;
      uint8_t *row_end = row_begin + width * 3;

      std::copy(row_begin, row_end, imagepixels);
      imagepixels += image->pitch;
    }
    SDL_UnlockSurface(image);
    SDL_SaveBMP(image, filename);
    SDL_FreeSurface( image );
  }
}
