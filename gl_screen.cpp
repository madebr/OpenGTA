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
#include <string>
#include "gl_screen.h"
#include "log.h"
#include "buffercache.h"
#include "m_exceptions.h"
#include "image_loader.h"

namespace OpenGL {
#ifndef DEFAULT_SCREEN_WIDTH
#define DEFAULT_SCREEN_WIDTH 640
#endif
#ifndef DEFAULT_SCREEN_HEIGHT
#define DEFAULT_SCREEN_HEIGHT 480
#endif
#ifndef DEFAULT_SCREEN_VSYNC
#define DEFAULT_SCREEN_VSYNC 0
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
    // 0: no vsync, 1: sdl, 2 native
    useVsync = DEFAULT_SCREEN_VSYNC;
  }

  void Screen::activate(Uint32 w, Uint32 h) {
    if (w)
      width = w;
    if (h)
      height = h;
    initSDL();
    resize(width, height);
    INFO << "activating screen: " << width << "x" << height << std::endl;
    initGL();
    setSystemMouseCursor(false);
  }

  void Screen::setupGlVars( float fov, float near_p, float far_p) {
    fieldOfView = fov;
    nearPlane = near_p;
    farPlane = far_p;
  }

  void Screen::setupVsync(size_t mode) {
    useVsync = mode;
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

    const char* sdl_err = SDL_GetError();
    if (strlen(sdl_err) > 0)
      WARN << "SDL_Init complained: " << sdl_err << std::endl;
    SDL_ClearError();

    const SDL_VideoInfo *vInfo = SDL_GetVideoInfo();

    if (vInfo == NULL)
      throw E_NOTSUPPORTED("SDL_GetVideoInfo failed: " + std::string(SDL_GetError()));

    if (vInfo->hw_available == 1)
      videoFlags |= SDL_HWSURFACE;
    else
      videoFlags |= SDL_SWSURFACE;

    if (vInfo->blit_hw)
      videoFlags |= SDL_HWACCEL;

    bpp = vInfo->vfmt->BitsPerPixel;
 
    INFO << "video-probe:" << std::endl <<
     " hw-surface: " << (vInfo->hw_available == 1 ? "on" : "off") << std::endl <<
     " hw-blit: " << (vInfo->blit_hw ? "on" : "off") << std::endl <<
     " bpp: " << int (bpp) << std::endl;

    size_t color_depth_triple[3];
    switch(bpp) {
      case 32:
      case 24:
        for (int i=0; i < 3; ++i)
          color_depth_triple[i] = 8;
        break;
      case 16:
        color_depth_triple[0] = 5;
        color_depth_triple[1] = 6;
        color_depth_triple[2] = 5;
        break;
      case 15:
        for (int i=0; i < 3; ++i)
          color_depth_triple[i] = 5;
        break;
      case 8:
        color_depth_triple[0] = 2;
        color_depth_triple[1] = 3;
        color_depth_triple[2] = 3;
        break;
      default:
        throw E_NOTSUPPORTED("Invalid bit-per-pixel setting");
    }

    SDL_GL_SetAttribute( SDL_GL_RED_SIZE,   color_depth_triple[0]);
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, color_depth_triple[1]);
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE,  color_depth_triple[2]);
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1);
#ifdef HAVE_SDL_VSYNC
    if (useVsync == 1) {
      SDL_GL_SetAttribute( SDL_GL_SWAP_CONTROL, 1);
      INFO << "enabling vertical sync:" << " SDL" << std::endl;
    }
#else
    if (useVsync == 1)
      WARN << "Cannot use SDL vsync - option disabled while compiling" << std::endl;
#endif

    sdl_err = SDL_GetError();
    if (strlen(sdl_err) > 0)
      ERROR << "setting sdl_gl attributes: " << sdl_err << std::endl;

  }

  void Screen::initGL() {
    GL_CHECKERROR;
    if (useVsync == 2) {
#ifdef LINUX
      int (*fp)(int) = (int(*)(int)) SDL_GL_GetProcAddress("glXSwapIntervalMESA");
      if (fp) {
        fp(1);
        INFO << "enabling vertical sync:" << " GLX" << std::endl;
      }
      else
        ERROR << "No symbol 'glXSwapIntervalMESA' found - cannot use GLX vsync" << std::endl;
#else
      typedef void (APIENTRY * WGLSWAPINTERVALEXT) (int);
      WGLSWAPINTERVALEXT wglSwapIntervalEXT = 
        (WGLSWAPINTERVALEXT) wglGetProcAddress("wglSwapIntervalEXT");
      if (wglSwapIntervalEXT)
      {
        wglSwapIntervalEXT(1); // set vertical synchronisation
        INFO << "enabling vertical sync:" << " WGL" << std::endl;
      }
      else
        ERROR << "No symbol 'wglSwapIntervalEXT' found - cannot use WGL vsync" << std::endl;
#endif
    }
    /*
    GLfloat LightAmbient[]  = { 0.1f, 0.1f, 0.1f, 1.0f };
    GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    GLfloat LightPosition[] = { 1.0f, 1.0f, 0.0f, 0.0f };
    */
    //glShadeModel( GL_SMOOTH );
    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
    glEnable( GL_DEPTH_TEST );
    /*
    glEnable( GL_LIGHTING );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
    glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
    glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
    glEnable( GL_LIGHT0 );
    */
    glEnable( GL_COLOR_MATERIAL);
    glCullFace(GL_BACK);
    //glPolygonMode(GL_FRONT, GL_FILL);
    //glPolygonMode(GL_BACK, GL_LINE);
    glEnable(GL_TEXTURE_2D);

    if (queryExtension("GL_EXT_texture_filter_anisotropic")) {
      GLfloat maxAniso = 1.0f;
      glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
      //if (maxAniso >= 2.0f)
      ImageUtil::supportedMaxAnisoDegree = maxAniso;
      INFO << "GL supports anisotropic filtering with degree: " << maxAniso << std::endl;
    }

    GL_CHECKERROR;
  }

  void Screen::resize(Uint32 w, Uint32 h) {
    if (h == 0)
      h = 1;
    surface = SDL_SetVideoMode(w, h, bpp, videoFlags);
    if (surface == NULL) {
        ERROR << "vide-mode: " << w << ", " << h << " bpp: " << bpp
              << " hw-surface: "
              << (((videoFlags & SDL_HWSURFACE) == SDL_HWSURFACE) ? "on"
                                                                  : "off")
              << " hw-blit: "
              << (((videoFlags & SDL_HWACCEL) == SDL_HWACCEL) ? "on" : "off")
              << std::endl;
        throw E_NOTSUPPORTED(SDL_GetError());
    }

    glViewport(0, 0, w, h);
    width = w;
    height = h;
    GL_CHECKERROR;
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
    uint8_t *pixels = Util::BufferCache::Instance().requestBuffer(width * height * 3);

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

  GLboolean Screen::queryExtension(const char *extName) {
    // from the 'Red Book'
    char *p = (char *) glGetString(GL_EXTENSIONS);
    char *end = p + strlen(p); 
    while (p < end) {
      size_t n = strcspn(p, " ");
      if ((strlen(extName)==n) && (strncmp(extName,p,n)==0)) {
        return GL_TRUE;
      }
      p += (n + 1);
    }
    return GL_FALSE;
  }
}
