#include <iostream>
#include <cmath>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include "common_sdl_gl.h"
#include "log.h"
#include "opengta.h"

extern SDL_Surface* screen;
GLfloat mapPos[2] = {0.0f, 0.0f};

OpenGTA::Map *map = NULL;

float slope_raw_data[45][5][4][3] = {
#include "slope1_data.h"
};

float lid_normal_data[45][3] = { 
#include "lid_normal_data.h"
};

int slope_idx = 0;
float angle = 0;

GLuint north, south, west, east, lid;

void on_exit() {
  SDL_Quit();
  if (map)
    delete map;
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
    case SDLK_LEFT:
      mapPos[0] -= 1.0f;
      break;
    case SDLK_RIGHT:
      mapPos[0] += 1.0f;
      break;
    case SDLK_UP:
      mapPos[1] += 1.0f;
      break;
    case SDLK_DOWN:
      mapPos[1] -= 1.0f;
      break;
    case 'a':
      glEnable(GL_CULL_FACE);
      printf("backfaces culled\n");
      break;
    case 'b':
      glDisable(GL_CULL_FACE);
      printf("all faces drawn\n");
      break;
    case '.':
      angle += 1.0f;
      break;
    case ',':
      angle -= 1.0f;
      break;
    case '+':
      slope_idx++;
      if (slope_idx > 44)
        slope_idx = 44;
      printf("now %i\n", slope_idx);
      break;
    case '-':
      slope_idx--;
      if (slope_idx < 0)
        slope_idx = 0;
      printf("now %i\n", slope_idx);
      break;
    default:
      break;
  }
}

void draw_slope(float size, int which) {
  float red = 0.4;
  float green = 0.7;
  float blue = 0.3;
#if 0
  
  float x1,x2,y1,y2,z1,z2 = 0.0f;
  x1 = slope_raw_data[which][0][0][0] - slope_raw_data[which][0][1][0];
  y1 = slope_raw_data[which][0][0][1] - slope_raw_data[which][0][1][1];
  z1 = slope_raw_data[which][0][0][2] - slope_raw_data[which][0][1][2];
  
  x2 = slope_raw_data[which][0][1][0] - slope_raw_data[which][0][2][0];
  y2 = slope_raw_data[which][0][1][1] - slope_raw_data[which][0][2][1];
  z2 = slope_raw_data[which][0][1][2] - slope_raw_data[which][0][2][2];
  
  float nx,ny,nz,vLen = 0.0f;
  nx = (y1 * z2) - (z1 * y2);
  ny = (z1 * x2) - (x1 * z2);
  nz = (x1 * y2) - (y1 * x2);
  vLen = sqrt( (nx * nx) + (ny * ny) + (nz * nz) );
  nx /= vLen;
  ny /= vLen;
  nz /= vLen;
  //std::cout << nx << " " << ny << " " << nz << std::endl;
#endif
  float nx = lid_normal_data[which][0];
  float ny = lid_normal_data[which][1];
  float nz = lid_normal_data[which][2];

  GLfloat lidTex[8] = {1.0f, 0.0f,  0.0f, 0.0f,  0.0f, 1.0f,  1.0f, 1.0f};
  //GLfloat lidTex[8] = {0.0f, 0.0f,  1.0f, 0.0f,  1.0f, 1.0f,  0.0f, 1.0f};
#ifdef GLTEX_HELPER
#undef GLTEX_HELPER
#endif
#define GLTEX_HELPER \
 glTexCoord2f(lidTex[jj], lidTex[jj+1]); jj += 2; if (jj > 6) { jj = 0; }
  for (int i=0; i<5; i++) {
    if (i == 0) {
      red = green = 0.0f;
      blue = 1.0f;
    }
    else if (i == 1) {
      red = blue = 0.0f;
      green = 1.0f;
    }
    else if (i == 2) {
      red = 1.0f;
      blue = green = 0.0f;
    }
    else if (i == 3) {
      red = 0.0f;
      blue = green = 1.0f;
    }
    else if (i == 4) {
      red = green = 1.0f;
      blue = 0.0f;
    }
    if (which == 41 && i == 1)
      continue;
    if (which == 42 && i == 2)
      continue;
    if (which == 43 && i == 4)
      continue;
    if (which == 44 && i == 3)
      continue;
    /*
    if (i == 0)
      glBindTexture(GL_TEXTURE_2D, lid);
    if (i == 1)
      glBindTexture(GL_TEXTURE_2D, north);
    if (i == 2)
      glBindTexture(GL_TEXTURE_2D, south);
    if (i == 3)
      glBindTexture(GL_TEXTURE_2D, east); // FIXME: flipped west<->east ???
    if (i == 4)
      glBindTexture(GL_TEXTURE_2D, west);
    */
    glBegin(GL_QUADS);
    glColor3f(red, green, blue);
    switch(i) {
      case 0:
        glNormal3f(nx, ny, nz);
        break;
      case 1:
        glNormal3f(0.0f, 0.0f, 1.0f);
        break;
      case 2:
        glNormal3f(0.0f, 0.0f, -1.0f);
        break;
      case 3:
        glNormal3f(-1.0f, 0.0f, 0.0f);
        break;
      case 4:
        glNormal3f(1.0f, 0.0f, 0.0f);
        break;
    }
    int jj = 0;
    for (int j=0; j < 4; j++) {/*
      switch(j) {
        case 0:
          //glNormal3f(0.0f, 0.0f, 1.0f);
          break;
        case 1:
          //glNormal3f(0.0f, -1.0f, 0.0f);
          break;
        case 2:
          //glNormal3f(0.0f, 1.0f, 0.0f);
          break;
      }*/
      //GLTEX_HELPER;
      glVertex3f(slope_raw_data[which][i][j][0],
          slope_raw_data[which][i][j][1],
          slope_raw_data[which][i][j][2]);
    }
    glEnd();
    #if 0
    if (i == 0) {
      glBegin(GL_LINES);
        glColor3f(0.0f, 0.0f, 0.5f);
      glVertex3f(0.5f, 1.0f, -0.5f);
        glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(0.5f+nx, 1.0f+ny, -0.5f+nz);
      glEnd();
    }
    if (i == 1) {
      glBegin(GL_LINES);
        glColor3f(0.0f, 0.5f, 0.0f);
      glVertex3f(0.5f, 0.5f, 0.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(0.5f, 0.5f, 1.0f);
      glEnd();
    }
    if (i == 2) {
      glBegin(GL_LINES);
        glColor3f(0.5f, 0.0f, 0.0f);
      glVertex3f(0.5f, 0.5f, -1.0f);
        glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(0.5f, 0.5f, -2.0f);
      glEnd();
    }
    if (i == 3) {
      glBegin(GL_LINES);
        glColor3f(0.0f, 0.5f, 0.5f);
      glVertex3f(0.0f, 0.5f, -0.5f);
        glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(-1.0f, 0.5f, -0.5f);
      glEnd();
    }
    if (i == 4) {
      glBegin(GL_LINES);
        glColor3f(0.5f, 0.5f, 0.0f);
      glVertex3f(1.0f, 0.5f, -0.5f);
        glColor3f(1.0f, 1.0f, 1.0f);
      glVertex3f(2.0f, 0.5f, -0.5f);
      glEnd();
    }
    #endif
  }
}

#if 0
void draw_cube(float size) {
  /* thanks to lesson 6 at Nehe */

  glDisable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glColor3f(1.0f, 0.0f, 1.0f);
  // Front Face
  glTexCoord2f(0.0f, 0.0f);
  glNormal3f(0.0f, 1.0f, 0.0f);
  glVertex3f(-size, -size,  size);// Bottom Left Of The Texture and Quad
  glTexCoord2f(1.0f, 0.0f); 
  glVertex3f( size, -size,  size);// Bottom Right Of The Texture and Quad
  glTexCoord2f(1.0f, 1.0f); 
  glVertex3f( size,  size,  size);// Top Right Of The Texture and Quad
  glTexCoord2f(0.0f, 1.0f); 
  glVertex3f(-size,  size,  size);// Top Left Of The Texture and Quad

  glColor3f(1.0f, 0.0f, 0.0f);
  // Back Face
  glTexCoord2f(1.0f, 0.0f); 
  glNormal3f(0.0f, -1.0f, 0.0f);
  glVertex3f(-size, -size, -size);// Bottom Right Of The Texture and Quad
  glTexCoord2f(1.0f, 1.0f); 
  glVertex3f(-size,  size, -size);// Top Right Of The Texture and Quad
  glTexCoord2f(0.0f, 1.0f); 
  glVertex3f( size,  size, -size);// Top Left Of The Texture and Quad
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f( size, -size, -size);// Bottom Left Of The Texture and Quad
  glColor3f(1.0f, 1.0f, 1.0f);
  // Top Face
  glTexCoord2f(0.0f, 1.0f); 
  glNormal3f(0.0f, 0.0f, 1.0f);
  glVertex3f(-size,  size, -size);// Top Left Of The Texture and Quad
  glTexCoord2f(0.0f, 0.0f); 
  glVertex3f(-size,  size,  size);// Bottom Left Of The Texture and Quad
  glTexCoord2f(1.0f, 0.0f); 
  glVertex3f( size,  size,  size);// Bottom Right Of The Texture and Quad
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f( size,  size, -size);// Top Right Of The Texture and Quad
  // Bottom Face
  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoord2f(1.0f, 1.0f);
  glNormal3f(0.0f, 0.0f, -1.0f);
  glVertex3f(-size, -size, -size);// Top Right Of The Texture and Quad
  glTexCoord2f(0.0f, 1.0f); 
  glVertex3f( size, -size, -size);// Top Left Of The Texture and Quad
  glTexCoord2f(0.0f, 0.0f); 
  glVertex3f( size, -size,  size);// Bottom Left Of The Texture and Quad
  glTexCoord2f(1.0f, 0.0f); 
  glVertex3f(-size, -size,  size);// Bottom Right Of The Texture and Quad
  // Right face
  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoord2f(1.0f, 0.0f); 
  glNormal3f(1.0f, 0.0f, 0.0f);
  glVertex3f( size, -size, -size);// Bottom Right Of The Texture and Quad
  glTexCoord2f(1.0f, 1.0f); 
  glVertex3f( size,  size, -size);// Top Right Of The Texture and Quad
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f( size,  size,  size);// Top Left Of The Texture and Quad
  glTexCoord2f(0.0f, 0.0f);
  glVertex3f( size, -size,  size);// Bottom Left Of The Texture and Quad
  // Left Face
  glColor3f(1.0f, 1.0f, 1.0f);
  glTexCoord2f(0.0f, 0.0f); 
  glNormal3f(-1.0f, 0.0f, 0.0f);
  glVertex3f(-size, -size, -size);// Bottom Left Of The Texture and Quad
  glTexCoord2f(1.0f, 0.0f);
  glVertex3f(-size, -size,  size);// Bottom Right Of The Texture and Quad
  glTexCoord2f(1.0f, 1.0f);
  glVertex3f(-size,  size,  size);// Top Right Of The Texture and Quad
  glTexCoord2f(0.0f, 1.0f);
  glVertex3f(-size,  size, -size);// Top Left Of The Texture and Quad
  glEnd();
}
#endif

void drawScene() {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glLoadIdentity();
  gluLookAt(5.0f, 5.0f, 5.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  glTranslatef(mapPos[0], mapPos[1], 0.0f);
/*
  for (int i=0; i<50; i++) {
  glPushMatrix();
  glTranslatef(0.0f, float(i), 0.0f);
  for (int j=0; j<50; j++) {
    glTranslatef(1.0f, 0.0f, 0.0f);
    float step_col = 0.0f;
    glColor3f(0.2f+step_col, 0.2f+step_col, 1.0f-step_col);
    PHYSFS_uint16 emptycount = map->getNumBlocksAt(j,i);
    glPushMatrix();
    for(int c=0; c < 6 - emptycount; c++) {
      draw_cube(0.5f);
      glTranslatef(0.0f, 0.0f, 1.0f);
      step_col += 0.2;
      glColor3f(0.2f+step_col, 0.1f+step_col, 1.0f-step_col);
    }
    glPopMatrix();
  }
  glPopMatrix();
  }*/
  //draw_cube(1.0f);
  glRotatef(angle, 0, 1, 0);
  glEnable(GL_TEXTURE_2D);
  draw_slope(1.0f, slope_idx);
  SDL_GL_SwapBuffers();
}

GLuint createGLTexture(GLsizei w, GLsizei h, const void* pixels) {
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glEnable(GL_COLOR_MATERIAL);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //      GL_LINEAR_MIPMAP_LINEAR);
  /*gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w,
      h, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  */
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  return tex;
}

void run_main() {
  SDL_Event event;
  int paused = 0;

  PHYSFS_init("mapview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);

/*
  SDL_Surface * tex = IMG_Load("lid.jpg");
  SDL_LockSurface(tex);
  lid = createGLTexture(tex->w, tex->h, tex->pixels);
  SDL_UnlockSurface(tex);
  SDL_FreeSurface(tex);
  tex = IMG_Load("north.jpg");
  SDL_LockSurface(tex);
  north = createGLTexture(tex->w, tex->h, tex->pixels);
  SDL_UnlockSurface(tex);
  SDL_FreeSurface(tex);
  tex = IMG_Load("south.jpg");
  SDL_LockSurface(tex);
  south = createGLTexture(tex->w, tex->h, tex->pixels);
  SDL_UnlockSurface(tex);
  SDL_FreeSurface(tex);
  tex = IMG_Load("west.jpg");
  SDL_LockSurface(tex);
  west = createGLTexture(tex->w, tex->h, tex->pixels);
  SDL_UnlockSurface(tex);
  SDL_FreeSurface(tex);
  tex = IMG_Load("east.jpg");
  SDL_LockSurface(tex);
  east = createGLTexture(tex->w, tex->h, tex->pixels);
  SDL_UnlockSurface(tex);
  SDL_FreeSurface(tex);
  */

  //glDisable(GL_COLOR_MATERIAL);

  glCullFace(GL_BACK);
  //glEnable(GL_CULL_FACE);
  //map = new OpenGTA::Map("NYC.CMP");
  
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
            ERROR << "Failed to set video mode after resize event" << std::endl;
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
  glDeleteTextures(1, &lid);
  glDeleteTextures(1, &north);
  glDeleteTextures(1, &south);
  glDeleteTextures(1, &west);
  glDeleteTextures(1, &east);

}
