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
float screen_gamma = 1.0f;

OpenGTA::Car * car = NULL;
Vector3D _p(4, 0.01f, 4);
OpenGTA::Pedestrian ped(Vector3D(0.5f, 0.5f, 0.5f), Vector3D(4, 0.01f, 4), 0xffffffff);
OpenGTA::SpriteObject::Animation pedAnim(0, 0);

OpenGL::DrawableFont m_font;

int frame_offset = 0;
int first_offset = 0;
int second_offset = 0;
int now_frame = 0;
bool play_anim = false;
unsigned int play_anim_time = 0;
int bbox_toggle = 0;
int texsprite_toggle = 0;
int c_c = 1;
int car_model = 0;
int car_remap = -1;
int car_last_model_ok = 0;
bool playWithCar = false;
uint32_t car_delta = 0;

int spr_type = (int)ped.sprType;
namespace OpenGTA {
  void ai_step_fake(OpenGTA::Pedestrian*) {
  }
}

  void on_exit() {
    if (car)
      delete car;
    SDL_Quit();
    PHYSFS_deinit();
  }

  void safe_try_model(uint8_t model_id) {
    if (car)
      delete car;
    car = NULL;
    try {
      car = new OpenGTA::Car(_p, 0, 0, model_id, car_remap);
    }
    catch (Util::UnknownKey & uk) {
      car = NULL;
      ERROR << "not a model" << std::endl;
      return;
    }
    car_last_model_ok = model_id;
  }

void run_init(const char*) {
  PHYSFS_init("mapview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  OpenGL::Screen::Instance().activate(640, 480);
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );

  OpenGTA::ActiveStyle::Instance().load(style_file);
  OpenGTA::ActiveStyle::Instance().get().setDeltaHandling(true);
  OpenGTA::MainMsgLookup::Instance().load("ENGLISH.FXT");

  m_font.loadFont("F_MTEXT.FON");
  m_font.setScale(1);
  glClearColor(1, 1, 1, 1);
  if (playWithCar)
    car = new OpenGTA::Car(_p, 0, 0, car_model);
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

const char* vtype2name(int vt) {
  switch(vt) {
    case 0:
      return "bus";
    case 3:
      return "motorcycle";
    case 4:
      return "car";
    case 8:
      return "train";
  }
  return "";
}

void drawScene(Uint32 ticks) {
  GL_CHECKERROR;
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  OpenGL::Screen::Instance().set3DProjection();
  OpenGL::Camera::Instance().update(ticks);

  if (playWithCar) {
    if (car) {
      car->update(ticks);
      OpenGTA::SpriteManager::Instance().draw(*car);
    }

    OpenGL::Screen::Instance().setFlatProjection();

    glPushMatrix();
    glTranslatef(10, 10, 0);

    std::string sprite_info;
    if (car != nullptr) {
      sprite_info = std::string { vtype2name(car->carInfo.vtype) }
            + " model: " + std::to_string(int(car_model)) + " name: "
            + OpenGTA::MainMsgLookup::Instance().get().getText(
                "car" + std::to_string(int(car_model)));
    } else {
      sprite_info = "not a model: " + std::to_string(int(car_model));
    }
    m_font.drawString(sprite_info);
    glPopMatrix();
  }
  else {
    if (play_anim && ticks > play_anim_time + 200) {
      now_frame++;
      if (now_frame > second_offset)
        now_frame = first_offset;
      ped.anim.firstFrameOffset = now_frame;
      play_anim_time = ticks;
    }
    OpenGTA::SpriteManager::Instance().draw(ped);

    OpenGL::Screen::Instance().setFlatProjection();

    glPushMatrix();
    glTranslatef(10, 10, 0);
    std::string sprite_info = std::string { spr_type_name(spr_type) }
        + " offset " + std::to_string(frame_offset);
    m_font.drawString(sprite_info);
    glPopMatrix();
  }

  SDL_GL_SwapBuffers();
  GL_CHECKERROR;
}

void handleKeyPress( SDL_keysym *keysym ) {
  OpenGL::Camera & cam = OpenGL::Camera::Instance();
  OpenGTA::GraphicsBase & style = OpenGTA::ActiveStyle::Instance().get();
  bool update_anim = false;
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case SDLK_TAB:
      c_c += 1; c_c %= 2;
      glClearColor(c_c, c_c, c_c, 0);
      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      break;
    case 'k':
      if (car_delta > 0)
        car_delta -= 1;
      if (car)
        car->delta = car_delta;
      break;
    case 'l':
      if (car_delta < 32)
        car_delta += 1;
      if (car) {
        car->delta = car_delta;
      }
      break;
    case '+':
      cam.translateBy(Vector3D(0, -0.5f, 0));
      break;
    case '-':
      cam.translateBy(Vector3D(0, 0.5f, 0));
      break;
    case '1':
      if (playWithCar) {
        if (car->animState.get_item(1))
          car->closeDoor(0);
        else
          car->openDoor(0);
      }
      break;
    case '2':
      if (playWithCar) {
        if (car->animState.get_item(2))
          car->closeDoor(1);
        else
          car->openDoor(1);
      }
      break;
    case '3':
      if (playWithCar) {
        if (car->animState.get_item(3))
          car->closeDoor(2);
        else
          car->openDoor(2);
      }
      break;
    case '4':
      if (playWithCar) {
        if (car->animState.get_item(4))
          car->closeDoor(3);
        else
          car->openDoor(3);
      }
      break;


    case ',':
      if (playWithCar) {
        car_model -= 1;
        if (car_model < 0)
          car_model = 0;
      }
      frame_offset -= 1;
      if (frame_offset < 0)
        frame_offset = 0;
      update_anim = true;
      break;
    case '.':
      if (playWithCar) {
        car_model += 1;
        if (car_model > 88)
          car_model = 88;
      }
      frame_offset += 1;
      if (frame_offset >= style.spriteNumbers.countByType(ped.sprType))
        frame_offset -= 1;
      update_anim = true;
      break;
    case 'n':
      if (playWithCar) {
        car_remap -= 1;
        if (car_remap < -1)
          car_remap = -1;
        INFO << "remap: " << int(car_remap) << std::endl;
      }
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
      if (playWithCar) {
        car_remap += 1;
        if (car_remap > 11)
          car_remap = 11;
        INFO << "remap: " << int(car_remap) << std::endl;
      }
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
    case 's':
      if (playWithCar) {
        car->setSirenAnim(true);
      }
      break;
    case SDLK_F2:
      bbox_toggle = (bbox_toggle ? 0 : 1);
      OpenGTA::SpriteManager::Instance().setDrawBBox(bbox_toggle);
      break;
    case SDLK_F3:
      texsprite_toggle = (texsprite_toggle ? 0 : 1);
      OpenGTA::SpriteManager::Instance().setDrawTexBorder(texsprite_toggle);
      break;
    case SDLK_F5:
      first_offset = frame_offset;
      std::cout << "First frame: " << first_offset << std::endl;
      break;
    case SDLK_F6:
      second_offset = frame_offset;
      std::cout << "Last frame: " << second_offset << std::endl;
      break;
    case SDLK_F7:
      play_anim = (play_anim ? false : true);
      if (play_anim)
        std::cout << "Playing: " << first_offset << " .. " << second_offset << std::endl;
      now_frame = first_offset;
      break;
    case SDLK_F8:
      playWithCar = playWithCar ? false : true;
      update_anim = true;
      break;
    default:
      break;
  }
  if (update_anim) {
    pedAnim.firstFrameOffset = frame_offset;
    ped.anim = pedAnim;
    if (playWithCar)
      safe_try_model(car_model);
  }
}

void usage(const char* a0) {
  std::cout << "USAGE: " << a0 << " [style-filename]" << std::endl;
  std::cout << std::endl << "Default is: STYLE001.GRY" << std::endl <<
    "Keys:" << std::endl <<
    " + - : zoom in/out" << std::endl <<
    " , . : previous/next frame offset" << std::endl <<
    " n m : previous/next sprite-type" << std::endl <<
    " tab : black/white background" << std::endl <<
    " F2  : toggle BBox drawn" << std::endl <<
    " F3  : toggle tex-border drawn" << std::endl <<
    " F5  : prepare animation: first-frame = current frame" << std::endl <<
    " F6  : prepare animation: last-frame  = current frame" << std::endl <<
    " F7  : toggle: play frames" << std::endl <<
    " F8  : toggle: special-car-mode" << std::endl << std::endl <<
    "In car-mode:" << std::endl <<
    " , . : choose model" << std::endl <<
    " n m : choose remap" << std::endl <<
    " 1, 2, 3, 4 : open car door (if exists)" << std::endl <<
    " s   : toggle siren anim (if exists)" << std::endl;
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

  OpenGL::Camera & cam = OpenGL::Camera::Instance();
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
          OpenGL::Screen::Instance().resize(event.resize.w, event.resize.h);
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
