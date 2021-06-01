#include <iostream>
#include <SDL.h>
#include <SDL_opengl.h>

#include "log.h"

extern int global_EC;

#ifndef VIEWGL_FOVY
#define VIEWGL_FOVY 60.0f
#endif
#ifndef VIEWGL_ZNEAR
#define VIEWGL_ZNEAR 0.1f
#endif
#ifndef VIEWGL_ZFAR
#define VIEWGL_ZFAR 250.0f
#endif

extern SDL_Surface* screen;
int videoFlags = 0;

/*
void ERROR(const char* s) {
  std::cerr << "Error" << s << std::endl;
  std::cerr << "* last SDL error was: " << SDL_GetError() << std::endl;
  global_EC = 1;
  exit(1);
}*/

int resize(int w, int h) {
  GLfloat ratio;
  if ( h == 0 )
    h = 1;
  ratio = ( GLfloat )w / ( GLfloat )h;
  glViewport( 0, 0, ( GLint )w, ( GLint )h );
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity( );
  //glRotatef(180, 0, 0, 1);
  gluPerspective( VIEWGL_FOVY, ratio, VIEWGL_ZNEAR, VIEWGL_ZFAR);

  //glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 300.0f); 
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity( );
  return(1);
}

void initVideo(int w, int h, int bpp) {
  const SDL_VideoInfo *videoInfo;
  /*
  SDL_Rect **modes;
  int i;
  
  videoInfo = SDL_GetVideoInfo( );
  modes=SDL_ListModes(videoInfo->vfmt, SDL_FULLSCREEN|SDL_HWSURFACE);
  if(modes == (SDL_Rect **)0){
  printf("No modes available!\n");
  exit(1);
  }

  if(modes == (SDL_Rect **)-1){
    printf("All resolutions available.\n");
  }
  else{
    printf("Available Modes\n");
    for(i=0;modes[i];++i)
      printf("  %d x %d\n", modes[i]->w, modes[i]->h);
  }
  */

  if (!videoInfo)
    ERROR << "VideoInfo query failed" << std::endl;
  videoFlags  = SDL_OPENGL;
  videoFlags |= SDL_GL_DOUBLEBUFFER;
  videoFlags |= SDL_HWPALETTE;
  //videoFlags |= SDL_RESIZABLE;
  //videoFlags |= SDL_FULLSCREEN;
  
  if ( videoInfo->hw_available ) {
    INFO << "Using HWSURFACE" << std::endl;
    videoFlags |= SDL_HWSURFACE;
  }
  else {
    INFO << "Using SWSURFACE" << std::endl;
    videoFlags |= SDL_SWSURFACE;
  }
  if ( videoInfo->blit_hw ) {
    INFO <<  "Using HWACCEL" << std::endl;
    videoFlags |= SDL_HWACCEL;
  }
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );

  screen = SDL_SetVideoMode( w, h, bpp, videoFlags );
  if (!screen)
    ERROR << "SDL failed to generate requested VideoSurface!" << std::endl;

  resize(w, h);
}

void initGL() {
  GLfloat LightAmbient[]  = { 0.5f, 0.5f, 0.5f, 1.0f };
  GLfloat LightDiffuse[]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  GLfloat LightPosition[] = { 500.0f, 200.0f, 300.0f, 1.0f };

  //glEnable( GL_TEXTURE_2D );
  glShadeModel( GL_SMOOTH );
  glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
  //glClearDepth( 1.0f );
  glEnable( GL_DEPTH_TEST );
  glEnable( GL_LIGHTING );
  //glDepthFunc( GL_LEQUAL );
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glLightfv( GL_LIGHT0, GL_AMBIENT, LightAmbient );
  glLightfv( GL_LIGHT0, GL_DIFFUSE, LightDiffuse );
  glLightfv( GL_LIGHT0, GL_POSITION, LightPosition );
  glEnable( GL_LIGHT0 );
  glEnable( GL_COLOR_MATERIAL);
  glCullFace(GL_BACK);
  glPolygonMode(GL_FRONT, GL_FILL);
  glPolygonMode(GL_BACK, GL_LINE);
}

/*

SDL_Surface* createRGBASurface(int w, int h) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define rmask 0xff000000 
#define gmask 0x00ff0000 
#define bmask 0x0000ff00 
#define amask 0x000000ff
#else 
#define rmask 0x000000ff 
#define gmask 0x0000ff00
#define bmask 0x00ff0000 
#define amask 0xff000000 
#endif 
  return SDL_CreateRGBSurface(SDL_SWSURFACE|SDL_SRCALPHA, 
      64, 64, 32, rmask, gmask, bmask, amask);
}

SDL_Surface* createRGBSurface(int w, int h) {
#if SDL_BYTEORDER == SDL_BIG_ENDIAN 
#define rmask 0xff000000 
#define gmask 0x00ff0000 
#define bmask 0x0000ff00 
#define amask 0x000000ff
#else 
#define rmask 0x000000ff 
#define gmask 0x0000ff00
#define bmask 0x00ff0000 
#define amask 0xff000000 
#endif 
  return SDL_CreateRGBSurface(SDL_SWSURFACE, 
      w, h, 24, rmask, gmask, bmask, amask);
}
*/
