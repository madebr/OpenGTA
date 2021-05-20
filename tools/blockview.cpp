#include <iostream>
#include <SDL_opengl.h>
#include "opengta.h"
#include "dataholder.h"
#include "gl_screen.h"
#include "gl_camera.h"
#include "log.h"

Uint32 arg_screen_w = 0;
Uint32 arg_screen_h = 0;
std::string map_file("NYC.CMP");
Vector3D e(3, 3, 3);
Vector3D c(0.5f, 0.5f, 0.5f);
Vector3D u(0, 1, 0);
uint8_t which = 0;
float r = 0;
bool wireframe = true;


extern int global_EC;
extern int global_Done;


const size_t numBlockTypes = 53;
const size_t numFaces = 5;

float slope_raw_data[numBlockTypes][numFaces][4][3] = {
#include "slope2_data.h"
};

#define SLOPE_RAW_DATA slope_raw_data


void on_exit() {
  SDL_Quit();
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
  else
    std::cout << "Goodbye" << std::endl;
}

void print_usage(const char*) {
}

void parse_args(int argc, char* argv[]) {
}

void handleKeyPress( SDL_keysym *keysym ) {
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case SDLK_LEFT:
      r -= 10;
      break;
    case SDLK_RIGHT:
      r += 10;
      break;
    case SDLK_SPACE:
      wireframe = (wireframe) ? false : true;
      break;
    case SDLK_UP:
      which++;
      if (which >= numBlockTypes)
        which = numBlockTypes - 1;
      INFO << "slope-type: " << int(which) << std::endl;
      break;
    case SDLK_DOWN:
      if (which > 0)
        which--;
      INFO << "slope-type: " << int(which) << std::endl;
      break;

    default:
      break;
  }
}

void drawScene() {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  OpenGL::Screen::Instance().set3DProjection();
  OpenGL::Camera::Instance().update(1);

  glRotatef(r, 0, 1, 0);
  glColor3f(0.5f, 1, 0.2f);
  // lid
  if (wireframe)
    glBegin(GL_LINE_STRIP);
  else
    glBegin(GL_QUADS);
  for (int j=0; j < 4; j++) {
    glVertex3f(SLOPE_RAW_DATA[which][0][j][0],
        SLOPE_RAW_DATA[which][0][j][1],
        SLOPE_RAW_DATA[which][0][j][2]);
  }
  if (wireframe)
    glVertex3f(SLOPE_RAW_DATA[which][0][0][0],
      SLOPE_RAW_DATA[which][0][0][1],
      SLOPE_RAW_DATA[which][0][0][2]);
  glEnd();

  glColor3f(0.1f, 0.5f, 1);
  // top
  if (which != 42 && which != 45) {
    if (wireframe)
      glBegin(GL_LINE_STRIP);
    else
      glBegin(GL_QUADS);
    for (int j=0; j < 4; j++) {
      glVertex3f(SLOPE_RAW_DATA[which][2][j][0],
          SLOPE_RAW_DATA[which][2][j][1],
          SLOPE_RAW_DATA[which][2][j][2]);
    }
    if (wireframe)
      glVertex3f(SLOPE_RAW_DATA[which][2][0][0],
        SLOPE_RAW_DATA[which][2][0][1],
        SLOPE_RAW_DATA[which][2][0][2]);
    glEnd();
  }

  glColor3f(1, 0.2f, 0.5f);
  // bottom
  if (which != 41 && which != 46) {
    if (wireframe)
      glBegin(GL_LINE_STRIP);
    else
      glBegin(GL_QUADS);
    for (int j=0; j < 4; j++) {
      glVertex3f(SLOPE_RAW_DATA[which][1][j][0],
          SLOPE_RAW_DATA[which][1][j][1],
          SLOPE_RAW_DATA[which][1][j][2]);
    }
    if (wireframe)
      glVertex3f(SLOPE_RAW_DATA[which][1][0][0],
        SLOPE_RAW_DATA[which][1][0][1],
        SLOPE_RAW_DATA[which][1][0][2]);
    glEnd();
  }

  glColor3f(0.4f, 0.1f, 0.6f);
  // left
  if (which != 44 && which != 48) {
    if (wireframe)
      glBegin(GL_LINE_STRIP);
    else
      glBegin(GL_QUADS);
    for (int j=0; j < 4; j++) {
      glVertex3f(SLOPE_RAW_DATA[which][3][j][0],
          SLOPE_RAW_DATA[which][3][j][1],
          SLOPE_RAW_DATA[which][3][j][2]);
    }
    if (wireframe)
      glVertex3f(SLOPE_RAW_DATA[which][3][0][0],
        SLOPE_RAW_DATA[which][3][0][1],
        SLOPE_RAW_DATA[which][3][0][2]);
    glEnd();
  }

  glColor3f(0.8f, 0.6f, 0.2f);
  // right
  if (which != 43 && which != 47) {
    if (wireframe)
      glBegin(GL_LINE_STRIP);
    else
      glBegin(GL_QUADS);
    for (int j=0; j < 4; j++) {
      glVertex3f(SLOPE_RAW_DATA[which][4][j][0],
          SLOPE_RAW_DATA[which][4][j][1],
          SLOPE_RAW_DATA[which][4][j][2]);
    }
    if (wireframe)
      glVertex3f(SLOPE_RAW_DATA[which][4][0][0],
        SLOPE_RAW_DATA[which][4][0][1],
        SLOPE_RAW_DATA[which][4][0][2]);
    glEnd();
  }

  SDL_GL_SwapBuffers();
}


void run_init(const char* prg) {
  PHYSFS_init("blockview");
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);

  OpenGL::Screen & screen = OpenGL::Screen::Instance();
  screen.activate(arg_screen_w, arg_screen_h);
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );

  OpenGTA::MainMsgLookup::Instance().load("ENGLISH.FXT");
  OpenGTA::ActiveMap::Instance().load(map_file);

  OpenGL::Camera::Instance().setVectors(e, c, u);

}

void run_main() {
  SDL_Event event;
  int paused = 0;

  PHYSFS_init("blockview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_CULL_FACE);

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
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        case SDL_MOUSEMOTION:
          break;
        default:
          break;
      }
    }
    if (!paused)
      drawScene();
  }
}
