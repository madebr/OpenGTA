#include <iostream>
#include <cmath>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "common_sdl_gl.h"
#include "opengta.h"
#include "gl_base.h"
#include "gl_font.h"

extern SDL_Surface* screen;
GLfloat mapPos[2] = {0.0f, 0.0f};
GLuint lid;
OpenGL::DrawableFont * font = NULL;

void on_exit() {
  if (font != NULL)
    delete font;
  SDL_Quit();
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
  else
    std::cout << "Goodbye" << std::endl;
}

void handleKeyPress( SDL_keysym *keysym ) {
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case 'a':
      glEnable(GL_CULL_FACE);
      printf("backfaces culled\n");
      break;
    case 'b':
      glDisable(GL_CULL_FACE);
      printf("all faces drawn\n");
      break;
    default:
      break;
  }
}

void drawScene() {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();

  // ...
  
  glEnable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0,640,0,480,-1,1);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
 
  font->drawString("HELLO 1234567890 Hello !");  
  
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  
  SDL_GL_SwapBuffers();
}

void run_main() {
  SDL_Event event;
  int paused = 0;

  PHYSFS_init("mapview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);

  font = new OpenGL::DrawableFont();

  font->loadFont("STREET1.FON");
  

  glCullFace(GL_BACK);
  
  while(!global_Done && !global_EC) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_ACTIVEEVENT:
          if (event.active.gain == 0)
            paused = 1;
          else
            paused = 0;
          break;
        case SDL_KEYDOWN:
          handleKeyPress(&event.key.keysym);
          break;
        case SDL_VIDEORESIZE:
          screen = SDL_SetVideoMode( event.resize.w,
              event.resize.h, 32, videoFlags );
          if (!screen)
            ERROR("Failed to set video mode after resize event");
          resize(event.resize.w, event.resize.h);
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        default:
          break;
      }
    }
    if (!paused)
      drawScene();
  }

}
