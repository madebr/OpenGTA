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
#include <iostream>
#include <sstream>
#include <SDL_opengl.h>
#include <unistd.h>
#include "gl_screen.h"
#include "opengta.h"
#include "log.h"
#include "spritemanager.h"
#include "m_exceptions.h"
#include "gl_camera.h"
#include "dataholder.h"
#include "gl_font.h"

extern int global_EC;
extern int global_Done;
std::string style_file("STYLE001.GRY");

OpenGTA::Pedestrian ped(Vector3D(0.5f, 0.5f, 0.5f), Vector3D(4, 0.01f, 4), 0xffffffff);
OpenGTA::SpriteObject::Animation pedAnim(0, 0);

OpenGL::DrawableFont m_font;

int frame_offset = 0;
int first_offset = 0;
int second_offset = 0;
int now_frame = 0;
bool play_anim = false;
int bbox_toggle = 0;
int texsprite_toggle = 0;

int spr_type = (int)ped.sprType;
namespace OpenGTA {
void ai_step_fake(OpenGTA::Pedestrian*) {
}
}

void on_exit() {
  SDL_Quit();
  PHYSFS_deinit();
}

void run_init(const char*) {
  PHYSFS_init("mapview");
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
  PHYSFS_addToSearchPath("gtadata.zip", 1);
  OpenGL::ScreenHolder::Instance().activate(640, 480);
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );

  OpenGTA::StyleHolder::Instance().load(style_file);

  m_font.loadFont("STREET1.FON");
  m_font.setScale(2);
}

const char* spr_type_name(int t) {
  switch(t) {
    case 0:
      return "arrow";
    case 1:
      return "digit";
    case 2:
      return "boat";
    case 3:
      return "box";
    case 4:
      return "bus";
    case 5:
      return "car";
    case 6:
      return "object";
    case 7:
      return "ped";
    case 8:
      return "speedo";
    case 9:
      return "tank";
    case 10:
      return "tr light";
    case 11:
      return "train";
    case 12:
      return "tr door";
    case 13:
      return "bike";
    case 14:
      return "tram";
    case 15:
      return "wbus";
    case 16:
      return "wcar";
    case 17:
      return "ex";
    case 18:
      return "tumcar";
    case 19:
      return "tumtruck";
    case 20:
      return "ferry";
  }
  return "???";
}

void drawScene(Uint32 ticks) {
  GL_CHECKERROR;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  OpenGL::ScreenHolder::Instance().set3DProjection();
  OpenGL::CameraHolder::Instance().update(ticks);

  if (play_anim) {
    pedAnim.firstFrameOffset = now_frame;
  }
  OpenGTA::SpriteManagerHolder::Instance().draw(ped);

  OpenGL::ScreenHolder::Instance().setFlatProjection();

  glPushMatrix();
  glTranslatef(10, 10, 0);
  std::ostringstream sprite_info_str;
  sprite_info_str << spr_type_name(spr_type) << " offset " << frame_offset;
  m_font.drawString(sprite_info_str.str());
  glPopMatrix();

  SDL_GL_SwapBuffers();
  GL_CHECKERROR;
}

void handleKeyPress( SDL_keysym *keysym ) {
  OpenGL::Camera & cam = OpenGL::CameraHolder::Instance();
  OpenGTA::GraphicsBase & style = OpenGTA::StyleHolder::Instance().get();
  bool update_anim = false;
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case '+':
      cam.translateBy(Vector3D(0, -0.5f, 0));
      break;
    case '-':
      cam.translateBy(Vector3D(0, 0.5f, 0));
      break;
    case ',':
      frame_offset -= 1;
      if (frame_offset < 0)
        frame_offset = 0;
      update_anim = true;
      break;
    case '.':
      frame_offset += 1;
      if (frame_offset >= style.spriteNumbers.countByType(ped.sprType))
        frame_offset -= 1;
      update_anim = true;
      break;
    case 'n':
      do {
        spr_type -= 1;
        if (spr_type < 0)
          spr_type = 0;
      } while (style.spriteNumbers.countByType(
        (OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes) spr_type) == 0);
      ped.sprType = (OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes)spr_type;
      frame_offset = 0;
      update_anim = 1;
      break;
    case 'm':
      do {
        spr_type += 1;
        if (spr_type > 20)
          spr_type = (int)ped.sprType;
      } while (style.spriteNumbers.countByType(
        (OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes) spr_type) == 0);
      ped.sprType = (OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes)spr_type;
      frame_offset = 0;
      update_anim = 1;
      break;
    case SDLK_F2:
      bbox_toggle = (bbox_toggle ? 0 : 1);
      OpenGTA::SpriteManagerHolder::Instance().setDrawBBox(bbox_toggle);
      break;
    case SDLK_F3:
      texsprite_toggle = (texsprite_toggle ? 0 : 1);
      OpenGTA::SpriteManagerHolder::Instance().setDrawTexBorder(texsprite_toggle);
      break;
    case SDLK_F5:
      first_offset = frame_offset;
      break;
    case SDLK_F6:
      second_offset = frame_offset;
      break;
    default:
      break;
  }
  if (update_anim) {
    pedAnim.firstFrameOffset = frame_offset;
    ped.anim = pedAnim;
  }
}

void usage(const char* a0) {
  std::cout << "USAGE: " << a0 << " [style-filename]" << std::endl;
  std::cout << std::endl << "Default is: STYLE001.GRY" << std::endl <<
  "Keys:" << std::endl <<
  " + - : zoom in/out" << std::endl <<
  " , . : previous/next frame offset" << std::endl <<
  " n m : previous/next sprite-type" << std::endl <<
  " F2  : toggle BBox drawn" << std::endl <<
  " F3  : toggle tex-border drawn" << std::endl;
}

void parse_args(int argc, char* argv[]) {
  int index;
  int c;

  opterr = 0;
#define VIEWER_FLAGS ""
  while ((c = getopt (argc, argv, VIEWER_FLAGS)) != -1)
    switch (c)
    { 
      case 'h':
      case '?':
        usage(argv[0]);
        exit(0);
      default:
        if (isprint (optopt))
          ERROR << "Unknown option `-" << char(optopt) << "'" << std::endl;
        else
          ERROR << "Unknown option character `" << optopt << "'" << std::endl;
        abort ();
    }

  for (index = optind; index < argc; index++)
    style_file = std::string(argv[index]);
}

void run_main() {
  SDL_Event event;

  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_CULL_FACE);

  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);

  OpenGL::Camera & cam = OpenGL::CameraHolder::Instance();
  cam.setVectors( Vector3D(4, 5, 4), Vector3D(4, 0.0f, 4.0f), Vector3D(0, 0, -1) );
  cam.setFollowMode(ped.pos);
  while(!global_Done && !global_EC) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_KEYDOWN:
          handleKeyPress(&event.key.keysym);
          break;
        case SDL_KEYUP:
//          handleKeyUp(&event.key.keysym);
          break;
        case SDL_VIDEORESIZE:
          OpenGL::ScreenHolder::Instance().resize(event.resize.w, event.resize.h);
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        case SDL_MOUSEMOTION:
          //std::cout << "Mouse move: x " << float(event.motion.x)/screen->w << " y " << float(event.motion.y)/screen->h << std::endl;
          break;
        default:
          break;
      }
    }
    Uint32 now_ticks = SDL_GetTicks();
    drawScene(now_ticks);
  }
}
