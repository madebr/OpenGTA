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
#include <iomanip>
#include <SDL_opengl.h>
#include <sstream>
#include <unistd.h>
#include "blockanim.h"
#include "dataholder.h"
#include "entity_controller.h"
#include "file_helper.h"
#include "gl_camera.h"
#include "gl_cityview.h"
#include "gl_font.h"
#include "gl_screen.h"
#include "gl_spritecache.h"
#include "gl_texturecache.h"
#include "id_sys.h"
#include "localplayer.h"
#include "log.h"
#include "m_exceptions.h"
#include "navdata.h"
#include "opengta.h"
#include "spritemanager.h"
#include "timer.h"
#ifdef WITH_LUA
#include "lua_addon/lua_vm.h"
#endif

#define getPedById getPed
#define removePedById removePed
#define addPed add

extern SDL_Surface* screen;
extern int global_EC;
extern int global_Done;
GLfloat mapPos[3] = {12.0f, 12.0f, 20.0f};

OpenGTA::CityView *city = NULL;
OpenGL::DrawableFont* m_font = NULL;

int city_num = 0;
const char*  styles_8[3] = { "STYLE001.GRY", "STYLE002.GRY", "STYLE003.GRY" };
const char* styles_24[3] = { "STYLE001.G24", "STYLE002.G24", "STYLE003.G24" };
const char* cities[3] = { "NYC.CMP", "SANB.CMP", "MIAMI.CMP" };
std::string specific_map;
std::string specific_style;

Uint32 num_frames_drawn = 0;
Uint32 fps = 0;
Uint32 last_tick;
Uint32 fps_last_tick;
Uint32 script_last_tick;
Uint32 arg_screen_w = 0;
Uint32 arg_screen_h = 0;
bool rotate = false;
bool cam_grav = false;
int tex_flip = 0;
int draw_arrows = 0;
int ped_anim = 0;
int bbox_toggle = 0;
int texsprite_toggle = 0;
int follow_toggle = 0;
OpenGTA::SpriteObject::Animation pedAnim(0, 0);
#ifdef OGTA_DEFAULT_GRAPHICS_G24
bool highcolor_data = true;
#else
bool highcolor_data = false;
#endif
bool full_screen = false;
bool player_toggle_run = false;

const char* script_file = NULL;
int paused = 0;
int next_station_zoom = 0;

/*
void ERROR(const char* s) {
  std::cerr << "Error" << s << std::endl;
  std::cerr << "* last SDL error was: " << SDL_GetError() << std::endl;
  global_EC = 1;
  exit(1);
}*/

void on_exit() {
  SDL_Quit();
  if (city)
    delete city;
  if (m_font)
    delete m_font;
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
  else
    std::cout << "Goodbye" << std::endl;
}

void print_usage(const char* argv0) {
  std::cout << "USAGE: " << argv0 << " [options] [city-num]" << std::endl <<
  std::endl <<
  "Options: " << std::endl <<
  " -l k : log-level (default: 0; 1, 2)" << std::endl <<
  " -c k : 0 = 8bit GRY, 1 = 24bit G24" << std::endl <<
  " -f   : fullscreen on program-start" << std::endl <<
  " -V   : show version & compile-time switches" << std::endl <<
  std::endl <<
  " -m map_file -g style_file : load specified files" << std::endl <<
  " -w width -h height        : screen dimension" << std::endl <<
  std::endl <<
  "City-num: 0 (default), 1, 2" << std::endl <<
  std::endl <<
  "The following environment variables are used when defined:" << std::endl <<
  " OGTA_DATA : PhysicsFS source for main data file lookup" << std::endl <<
  " OGTA_HOME : unused - will be config/save dir" << std::endl <<
  " OGTA_MOD  : PhysicsFS source to override main data files" << std::endl;
}

void print_version_info() {
#define PRINT_FORMATED(spaces) std::setw(spaces) << std::left <<
#define PRINT_OFFSET PRINT_FORMATED(18)
  std::cout << PRINT_OFFSET "OpenGTA version:" << OGTA_VERSION_INFO << std::endl <<
  PRINT_OFFSET "platform:" << OGTA_PLATFORM_INFO << std::endl << 

  PRINT_OFFSET "Lua support:" << 
#ifdef WITH_LUA
  "yes [" << LUA_RELEASE << "]" <<
#else
  "no" <<
#endif
  std::endl <<
  PRINT_OFFSET "sound support:" <<
#ifdef WITH_SOUND
  "yes" <<
#else
  "no" <<
#endif
  std::endl <<
  PRINT_OFFSET "vsync support:" <<
#ifdef HAVE_SDL_VSYNC
  "yes" <<
#else
  "no" <<
#endif
  std::endl <<

#ifdef OGTA_DEFAULT_DATA_PATH
  PRINT_OFFSET "data-path:" << "[" << OGTA_DEFAULT_DATA_PATH << "]" << std::endl <<
#endif
#ifdef OGTA_DEFAULT_MOD_PATH
  PRINT_OFFSET "mod-path:" << "[" << OGTA_DEFAULT_MOD_PATH << "]" << std::endl <<
#endif
  PRINT_OFFSET "default graphics:" <<
#ifdef OGTA_DEFAULT_GRAPHICS_G24
  "G24 - 24 bit" <<
#else
  "GRY - 8 bit" <<
#endif
  std::endl <<

  PRINT_OFFSET "compiler: " << USED_GCC_VERSION
  << std::endl;
}

void parse_args(int argc, char* argv[]) {
  int index;
  int c;

  opterr = 0;

#ifdef WITH_LUA
#define VIEWER_FLAGS "s:w:h:c:m:g:l:fV"
#else
#define VIEWER_FLAGS "w:h:c:m:g:l:fV"
#endif
  while ((c = getopt (argc, argv, VIEWER_FLAGS)) != -1)
    switch (c)
    {
#ifdef WITH_LUA
      case 's':
        script_file = std::string(optarg);
        break;
#endif
      case 'c':
        highcolor_data = atoi(optarg); 
        break;
      case 'm':
        specific_map = std::string(optarg);
        break;
      case 'g':
        specific_style = std::string(optarg);
        break;
      case 'w':
        arg_screen_w = atoi(optarg);
        break;
      case 'h':
        arg_screen_h = atoi(optarg);
        break;
      case 'l':
        Util::Log::setOutputLevel(atoi(optarg));
        break;
      case 'f':
        full_screen = true;
        break;
      case 'V':
        print_version_info();
        exit(0);
        break;
      default:
        if (optopt == '?') {
          print_usage(argv[0]);
          exit(0);
        }
        else if (isprint (optopt))
          ERROR << "Unknown option `-" << char(optopt) << "'" << std::endl;
        else
          ERROR << "Unknown option character `" << optopt << "'" << std::endl;
        print_usage(argv[0]);
        exit(1);
    }

  for (index = optind; index < argc; index++)
    city_num = atoi(argv[index]);

  if (city_num > 2) {
    ERROR << "Invalid city number: " << city_num << std::endl;
    exit(1);
  }
}

void run_init(const char* prg_name) {
  // physfs
  PHYSFS_init(prg_name);

  // physfs-ogta
  Util::FileHelper & fh = GET_FILE_HELPER;
  if (fh.existsInSystemFS(fh.getBaseDataPath())) {
    PHYSFS_addToSearchPath(GET_FILE_HELPER.getBaseDataPath().c_str(), 1);
  }
  else {
    WARN << "Could not load data-source: " << fh.getBaseDataPath() << std::endl;
  }
  
  PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);

  if (fh.existsInSystemFS(fh.getModDataPath()))
    PHYSFS_addToSearchPath(GET_FILE_HELPER.getModDataPath().c_str(), 0);


  OpenGL::Screen & screen = OpenGL::ScreenHolder::Instance();
#ifdef WITH_LUA
  if (fh.existsInVFS("config")) {
    char* config_as_string = (char*)fh.bufferFromVFS(
      fh.openReadVFS("config"));
    
    OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVMHolder::Instance();
    try {
      vm.runString(config_as_string);
    }
    catch (const Util::ScriptError & e) {
      std::cerr << "Error in config-file: " << e.what() << std::endl;
      global_EC = 1;
      exit(1);
    }

    try {
      Uint32 sw = vm.getGlobalInt("screen_width");
      if (!arg_screen_w)
        arg_screen_w = sw;
    }
    catch (const Util::ScriptError & e) {}
    try {
      Uint32 sh = vm.getGlobalInt("screen_height");
      if (!arg_screen_h)
        arg_screen_h = sh;
    }
    catch (const Util::ScriptError & e) {}
    try {
      Uint32 sh = vm.getGlobalInt("full_screen");
      if (!full_screen)
        full_screen = sh;
    }
    catch (const Util::ScriptError & e) {}

    float fov = screen.getFieldOfView();
    float np = screen.getNearPlane();
    float fp = screen.getFarPlane();
    try {
      fov = vm.getGlobalFloat("field_of_view");
    }
    catch (const Util::ScriptError & e) {}
    try {
      np = vm.getGlobalFloat("near_plane");
    }
    catch (const Util::ScriptError & e) {}
    try {
      fp = vm.getGlobalFloat("far_plane");
    }
    catch (const Util::ScriptError & e) {}
    screen.setupGlVars(fov, np, fp);

  }
#endif

  if ((arg_screen_h && !arg_screen_w) || (!arg_screen_h && arg_screen_w)) {
    WARN << "Invalid screen specified: " << arg_screen_w << "x" <<
      arg_screen_h << " - using default" << std::endl;
    arg_screen_h = 0; arg_screen_w = 0;
  }
  
  screen.setFullScreenFlag(full_screen);
  screen.activate(arg_screen_w, arg_screen_h);
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
}


void print_position() {
  Vector3D & v = OpenGL::CameraHolder::Instance().getCenter();
  Vector3D & e = OpenGL::CameraHolder::Instance().getEye();
  Vector3D & u = OpenGL::CameraHolder::Instance().getUp();
  if (!city->getViewMode()) {
  INFO << cities[city_num] << ": " << city->getCurrentSector()->getFullName() << std::endl <<
  "camera.setCenter(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl <<
  "camera.setEye(" << e.x << ", " << e.y << ", " << e.z << ")" << std::endl <<
  "camera.setUp(" << u.x << ", " << u.y << ", " << u.z << ")" << std::endl <<
  "city_view:setVisibleRange(" << city->getVisibleRange() << ")" << std::endl <<
  "city_view:setTopDownView( false )" << std::endl;
  }
  else {
    GLfloat* cp = city->getCamPos();
    INFO << cities[city_num] << ": " << city->getCurrentSector()->getFullName() << std::endl <<
    "city_view:setCamPosition(" << cp[0] << ", " << cp[1] << ", " << cp[2] << ")" << std::endl <<
    "city_view:setVisibleRange(" << city->getVisibleRange() << ")" << std::endl <<
    "city_view:setTopDownView( true )" << std::endl;

  }

}

void handleKeyUp( SDL_keysym *keysym) {
  switch ( keysym->sym ) {
    case 'j':
      OpenGTA::LocalPlayer::Instance().getCtrl().releaseTurnLeft();
      //OpenGTA::LocalPlayer::Instance().turn = 0;
      //OpenGTA::LocalPlayer::Instance().setTurn(0);
      break;
    case 'l':
      OpenGTA::LocalPlayer::Instance().getCtrl().releaseTurnRight();
      //OpenGTA::LocalPlayer::Instance().turn = 0;
      //OpenGTA::LocalPlayer::Instance().setTurn(0);
      break;
    case 'i':
      OpenGTA::LocalPlayer::Instance().getCtrl().releaseMoveForward();
      //OpenGTA::LocalPlayer::Instance().move = 0;
      //OpenGTA::LocalPlayer::Instance().setMove(0);
      break;
    case 'k':
      OpenGTA::LocalPlayer::Instance().getCtrl().releaseMoveBack();
      //OpenGTA::LocalPlayer::Instance().move = 0;
      //OpenGTA::LocalPlayer::Instance().setMove(0);
      break;
    case SDLK_LCTRL:
      OpenGTA::LocalPlayer::Instance().getCtrl().setFireWeapon(false);
      break;
    default:
      break;
  }
}

void draw_mapmode();

void create_ped_at(const Vector3D v) {
  OpenGTA::Pedestrian p(Vector3D(0.3f, 0.5f, 0.3f), v, 0xffffffff);
  p.remap = OpenGTA::StyleHolder::Instance().get().getRandomPedRemapNumber();
  INFO << "using remap: "  << p.remap << std::endl;
  OpenGTA::Pedestrian & pr = OpenGTA::SpriteManagerHolder::Instance().addPed(p);
  pr.switchToAnim(1);
  OpenGTA::LocalPlayer::Instance().setCtrl(pr.m_control);
  //pr.m_control = &OpenGTA::LocalPlayer::Instance();
  //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).giveItem(1, 255);
}

void explode_ped() {
  try {
  OpenGTA::Pedestrian & ped = OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff);
  Vector3D p(ped.pos);
  p.y += 0.2f;
  OpenGTA::SpriteManagerHolder::Instance().createExplosion(p);
  }
  catch (Util::UnknownKey & e) {
    WARN << "Cannot place explosion - press F4 to switch to player-mode first!" << std::endl;
  }
}

void zoomToTrain(int k) {
/*
  OpenGTA::TrainSegment & ts = OpenGTA::SpriteManagerHolder::Instance().getTrainById(k);
  Vector3D p(ts.pos);
  p.y += 9;
  OpenGL::CameraHolder::Instance().interpolate(p, 1, 30000);
*/

}

#include "cell_iterator.h"

namespace OpenGTA {
  void ai_step_fake(OpenGTA::Pedestrian *p) {
    try {
    OpenGTA::Pedestrian & pr = OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff);
    float t_angle = Util::xz_angle(p->pos, pr.pos);
    //INFO << "dist " << Util::distance(p->pos, pr.pos) << std::endl;
    //INFO << "angle " << t_angle << std::endl;
    //INFO << "myrot: " << p->rot << std::endl;
    if (Util::distance(p->pos, pr.pos) > 3) {
      p->m_control.setTurnLeft(false);
      p->m_control.setTurnRight(false);
      if (t_angle > p->rot)
        p->m_control.setTurnLeft(true);
      else
        p->m_control.setTurnRight(true);
    }
    else {
      p->m_control.setMoveForward(true);
      int k = rand() % 5;
      if (k == 0) {
        p->m_control.setTurnLeft(false);
        p->m_control.setTurnRight(false);
      }
      else if (k == 1) {
        p->m_control.setTurnLeft(true);
        p->m_control.setTurnRight(false);
      }
      else if (k == 2) {
        p->m_control.setTurnLeft(false);
        p->m_control.setTurnRight(true);
      }
    }
    }
    catch (Util::UnknownKey & e) {
    }

  }
}

#include "id_sys.h"
void add_auto_ped() {
  try {
  OpenGTA::Pedestrian & pr = OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff);
  int id = OpenGTA::TypeIdBlackBox::requestId();
  Vector3D v(pr.pos);
  v.y += 0.9f;
  //INFO << v.x << " " << v.y << " " << v.z << std::endl;
  Sint16 remap = OpenGTA::StyleHolder::Instance().get().getRandomPedRemapNumber();
  OpenGTA::Pedestrian p(Vector3D(0.3f, 0.5f, 0.3f), v, id, remap);
  OpenGTA::SpriteManagerHolder::Instance().addPed(p);
  OpenGTA::Pedestrian & pr2 = OpenGTA::SpriteManagerHolder::Instance().getPedById(id);
  pr2.switchToAnim(1);
  INFO << "now " << OpenGTA::SpriteManagerHolder::Instance().getNum<OpenGTA::Pedestrian>() << " peds " << std::endl;

  //pr2.m_control = &OpenGTA::nullAI;
  }
  catch (Util::UnknownKey & e) {
    WARN << "Cannot place peds now - press F4 to switch to player-mode first!" << std::endl;
  }
}

void toggle_player_run() {
  OpenGTA::PedController * pc = &OpenGTA::LocalPlayer::Instance().getCtrl();

  if (!pc) {
    WARN << "no player yet!" << std::endl;
    return;
  }
  if (!pc->getRunning()) 
    pc->setRunning(true);
  else
    pc->setRunning(false);
}

void handleKeyPress( SDL_keysym *keysym ) {
  GLfloat* cp = city->getCamPos();
  mapPos[0] = cp[0]; mapPos[1] = cp[1]; mapPos[2] = cp[2];
  OpenGL::Camera & cam = OpenGL::CameraHolder::Instance();
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    case SDLK_LEFT:
      mapPos[0] -= 1.0f;
      cam.translateBy(Vector3D(-1, 0, 0));
      break;
    case SDLK_RIGHT:
      mapPos[0] += 1.0f;
      cam.translateBy(Vector3D(1, 0, 0));
      break;
    case SDLK_UP:
      mapPos[2] -= 1.0f;
      cam.translateBy(Vector3D(0, 0, -1));
      break;
    case SDLK_DOWN:
      mapPos[2] += 1.0f;
      cam.translateBy(Vector3D(0, 0, 1));
      break;
    case SDLK_SPACE:
      cam.setSpeed(0.0f);
      break;
    case SDLK_F1:
      //cam.interpolate(Vector3D(254, 9, 254), 1, 20000);
      
      //zoomToTrain(next_station_zoom++);
      //if (next_station_zoom >= OpenGTA::SpriteManagerHolder::Instance().trainSystem.getNumTrains())
      //  next_station_zoom = 0;
      break;
    case SDLK_F2:
      bbox_toggle = (bbox_toggle ? 0 : 1);
      OpenGTA::SpriteManagerHolder::Instance().setDrawBBox(bbox_toggle);
      break;
    case SDLK_F3:
      texsprite_toggle = (texsprite_toggle ? 0 : 1);
      OpenGTA::SpriteManagerHolder::Instance().setDrawTexBorder(texsprite_toggle);
      break;
    case SDLK_F4:
      follow_toggle = (follow_toggle ? 0 : 1);
      if (follow_toggle) {
        SDL_EnableKeyRepeat( 0, SDL_DEFAULT_REPEAT_INTERVAL );
        city->setViewMode(false);
        Vector3D p(cam.getEye());
        create_ped_at(p);
        cam.setVectors( Vector3D(p.x, 10, p.z), Vector3D(p.x, 9.0f, p.z), Vector3D(0, 0, -1) );
        cam.setFollowMode(OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).pos);
        cam.setCamGravity(true);
      }
      else {
        SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
        cam.setVectors(cam.getEye(), 
          Vector3D(cam.getEye() + Vector3D(1, -1, 1)), Vector3D(0, 1, 0));
        cam.setCamGravity(false);
        cam.releaseFollowMode();
        OpenGTA::SpriteManagerHolder::Instance().removePedById(0xffffffff);
        OpenGTA::SpriteManagerHolder::Instance().removeDeadStuff();
      }
      break;
    case SDLK_F5:
      draw_arrows = (draw_arrows ? 0 : 1);
      city->setDrawHeadingArrows(draw_arrows);
      break;
    case SDLK_F6:
      draw_mapmode();
      break;
    case SDLK_F7:
      explode_ped();
      break;
    case SDLK_F8:
      add_auto_ped();
      break;
    case SDLK_F9:
      city->setDrawTextured(city->getDrawTextured() ? 0 : 1);
      break;
    case SDLK_F10:
      city->setDrawLines(city->getDrawLines() ? 0 : 1);
      break;
    case SDLK_LSHIFT:
      toggle_player_run();
      break;
    /*
    case SDLK_F6:
      tex_flip = (tex_flip ? 0 : 1);
      INFO << "flipping: " << tex_flip << std::endl;
      city->setTexFlipTest(tex_flip);
      break;
    */
    case SDLK_LCTRL:
      OpenGTA::LocalPlayer::Instance().getCtrl().setFireWeapon();
      break;
    case '1':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(1);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(1);
      break;
    case '2':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(2);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(2);
      break;
    case '3':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(3);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(3);
      break;
    case '4':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(4);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(4);
      break;
    case '5':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(5);
      break;
    case '6':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(6);
      break;
    case '7':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(7);
      break;
    case '8':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(8);
      break;
    case '9':
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(9);
      /*
      ped_anim -= 1; if (ped_anim < 0) ped_anim = 0;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      break;
    case '0':
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(0);
      /*
      ped_anim += 1; if (ped_anim > 200) ped_anim = 200;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      //OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(0);
      break;
    case 'w':
      cam.setSpeed(0.2f);
      break;
    case 's':
      cam.setSpeed(-0.2f);
      break;
    case 'j':
      OpenGTA::LocalPlayer::Instance().getCtrl().setTurnLeft();
      break;
    case 'l':
      OpenGTA::LocalPlayer::Instance().getCtrl().setTurnRight();
      break;
    case 'i':
      OpenGTA::LocalPlayer::Instance().getCtrl().setMoveForward();
      break;
    case 'k':
      OpenGTA::LocalPlayer::Instance().getCtrl().setMoveBack();
      break;
    case 'f':
      OpenGL::ScreenHolder::Instance().toggleFullscreen();
#ifdef WIN32
      city->resetTextures();
      m_font->resetTextures();
      OpenGL::SpriteCacheHolder::Instance().clearAll();
#endif
      break;
    case 'r':
      rotate = (rotate) ? false : true;
      cam.setRotating(rotate);
      break;
    case 'g':
      cam_grav = (cam_grav) ? false : true;
      cam.setCamGravity(cam_grav);
      break;
    case 't':
      mapPos[0] = mapPos[2] = 128;
      mapPos[1] = 230;
      city->setVisibleRange(128);
      break;
    case 'p':
      print_position();
      break;
    case '+':
      mapPos[1] += 1.0f;
      cam.translateBy(Vector3D(0, 1, 0));
      break;
    case '-':
      mapPos[1] -= 1.0f;
      cam.translateBy(Vector3D(0, -1, 0));
      break;
    case 'x':
      city->setViewMode(false);
      city->setVisibleRange(city->getVisibleRange() * 2);
      break;
    case 'y':
      break;
    case 'z':
      city->setViewMode(true);
      city->setVisibleRange(city->getVisibleRange() / 2);
      break;
    case '.':
      city->setVisibleRange(city->getVisibleRange()-1);
      INFO << " new visible range " << city->getVisibleRange() << std::endl;
      break;
    case ',':
      city->setVisibleRange(city->getVisibleRange()+1);
      INFO << " new visible range " << city->getVisibleRange() << std::endl;
      break;
    case SDLK_PRINT:
      OpenGL::ScreenHolder::Instance().makeScreenshot("screenshot.bmp"); 
      break;
    default:
      return; 
  }
  city->setPosition(mapPos[0], mapPos[1], mapPos[2]);

}

void drawScene(Uint32 ticks) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  OpenGL::ScreenHolder::Instance().set3DProjection();
  city->draw(ticks);

  OpenGL::ScreenHolder::Instance().setFlatProjection();

  glPushMatrix();
  glTranslatef(10, 10, 0);
  m_font->drawString(city->getCurrentSector()->getFullName());
  glPopMatrix();
  glPushMatrix();
  glTranslatef(5, 50, 0);
  std::ostringstream strstr;
  strstr << fps << " fps";
  m_font->drawString(strstr.str());
  glPopMatrix();

  num_frames_drawn += 1;

  SDL_GL_SwapBuffers();
}

void draw_mapmode() {
  SDL_Event event;
  OpenGL::PagedTexture map_tex = city->renderMap2Texture();
  bool done_map = false;
  OpenGL::Screen & screen = OpenGL::ScreenHolder::Instance();
  screen.setSystemMouseCursor(true);
  GLfloat _scale = 1;
  GLfloat dx = 0;
  GLfloat dy = 0;
  while(!done_map) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_QUIT:
          done_map = true;
          break;
        case SDL_KEYDOWN:
          switch(event.key.keysym.sym) {
            case SDLK_ESCAPE:
              done_map = true;
              break;
            case '-':
              _scale += 0.1f;
              break;
            case '+':
              _scale -= 0.1f;
              break;
            case SDLK_LEFT:
              dx -= 0.1f;
              break;
            case SDLK_RIGHT:
              dx += 0.1f;
              break;
            case SDLK_UP:
              dy += 0.1f;
              break;
            case SDLK_DOWN:
              dy -= 0.1f;
              break;
            default:
              break;
          }
          break;
        case SDL_MOUSEMOTION:
//          std::cout << "Mouse move: x " << event.motion.x << " y " << 
//            event.motion.y << std::endl;
          break;
        default:
          break;
      }
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screen.setFlatProjection();
    glBindTexture(GL_TEXTURE_2D, map_tex.inPage);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScalef(_scale, _scale, 1);
    glTranslatef(dx, dy, 0);

    glBegin(GL_QUADS);
    glTexCoord2f(map_tex.coords[0].u, map_tex.coords[0].v);
    glVertex2i(0, 0);
    glTexCoord2f(map_tex.coords[1].u, map_tex.coords[0].v);
    glVertex2i(screen.getWidth(), 0);
    glTexCoord2f(map_tex.coords[1].u, map_tex.coords[1].v);
    glVertex2i(screen.getWidth(), screen.getHeight());
    glTexCoord2f(map_tex.coords[0].u, map_tex.coords[1].v);
    glVertex2i(0, screen.getHeight());
    glEnd();

    SDL_GL_SwapBuffers();
    SDL_Delay(20);

  }
  screen.setSystemMouseCursor(false);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  // the texture class doesn't cleanup!
  glDeleteTextures(1, &map_tex.inPage); 
}

#if 0
void parse_args(int argc, char* argv[]) {
  int index;
  int c;

  opterr = 0;

#ifdef WITH_LUA
#define VIEWER_FLAGS "s:w:h:c:m:g:l:f"
#else
#define VIEWER_FLAGS "w:h:c:m:g:l:f"
#endif
  while ((c = getopt (argc, argv, VIEWER_FLAGS)) != -1)
    switch (c)
    {
#ifdef WITH_LUA
      case 's':
        script_file = optarg;
        break;
#endif
      case 'c':
        highcolor_data = atoi(optarg); 
        break;
      case 'm':
        specific_map = std::string(optarg);
        break;
      case 'g':
        specific_style = std::string(optarg);
        break;
      case 'w':
        arg_screen_w = atoi(optarg);
        break;
      case 'h':
        arg_screen_h = atoi(optarg);
        break;
      case 'l':
        Util::Log::setOutputLevel(atoi(optarg));
        break;
      case 'f':
        full_screen = true;
        break;
      case '?':
        if (isprint (optopt))
          ERROR << "Unknown option `-" << char(optopt) << "'" << std::endl;
        else
          ERROR << "Unknown option character `" << optopt << "'" << std::endl;
      default:
        abort ();
    }

  for (index = optind; index < argc; index++)
    city_num = atoi(argv[index]);

  if (city_num > 2) {
    ERROR << "Invalid city number: " << city_num << std::endl;
    exit(1);
  }
}

void run_init(const char* prgname) {
  PHYSFS_init("mapview");
  //PHYSFS_addToSearchPath("gtadata.zip", 1);
  Util::FileHelper & fh = GET_FILE_HELPER;
  if (fh.existsInSystemFS(fh.getBaseDataPath()))
    PHYSFS_addToSearchPath(GET_FILE_HELPER.getBaseDataPath().c_str(), 1);
  else {
    WARN << "Could not load data-source: " << fh.getBaseDataPath() <<" -- fallback to current directory"<< std::endl;
    PHYSFS_addToSearchPath(PHYSFS_getBaseDir(), 1);
  }
  if (fh.existsInSystemFS(fh.getModDataPath()))
    PHYSFS_addToSearchPath(GET_FILE_HELPER.getModDataPath().c_str(), 0);
  
  OpenGL::Screen & screen = OpenGL::ScreenHolder::Instance();
  screen.setFullScreenFlag(full_screen);
  screen.activate(arg_screen_w, arg_screen_h);
  //SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
}
#endif

void run_main() {
  SDL_Event event;
  OpenGTA::MainMsgHolder::Instance().load("ENGLISH.FXT");

  m_font = new OpenGL::DrawableFont();
  m_font->loadFont("F_MTEXT.FON");
  m_font->setScale(1);

  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_CULL_FACE);

  //glEnable(GL_BLEND);
  //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);

  city = new OpenGTA::CityView();
  if (specific_map.size() > 0 && specific_style.size() > 0) {
    city->loadMap(specific_map, specific_style);
  }
  else {
    if (highcolor_data)
      city->loadMap(cities[city_num], styles_24[city_num]);
    else
      city->loadMap(cities[city_num], styles_8[city_num]);
  }
  city->setPosition(mapPos[0], mapPos[1], mapPos[2]);

  OpenGL::Camera & cam = OpenGL::CameraHolder::Instance(); 
  //cam.setVectors( Vector3D(4, 10, 4), Vector3D(4, 0.0f, 4.0f), Vector3D(0, 0, -1) );
  cam.setVectors( Vector3D(12, 20, 12), Vector3D(13.0f, 19.0f, 13.0f), Vector3D(0, 1, 0) );

  
#ifdef TIMER_OPENSTEER_CLOCK
  Timer & timer = TimerHolder::Instance();
  timer.update();
  last_tick = timer.getRealTime();
  //timer.setSimulationRunning(true);
#else
  last_tick = SDL_GetTicks();
#endif

#ifdef WITH_LUA
  OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVMHolder::Instance();
  vm.setCityView(*city);
  if (script_file)
    vm.runFile(script_file);
  //vm.runString("function game_tick() print('tick...') end");
  bool vm_tick_ok = true;
  script_last_tick = last_tick;
#endif


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
        case SDL_KEYUP:
          handleKeyUp(&event.key.keysym);
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
#ifdef TIMER_OPENSTEER_CLOCK
    timer.update();
    Uint32 now_ticks = timer.getRealTime();
#else
    Uint32 now_ticks = SDL_GetTicks();
#endif
    OpenGTA::SpriteManagerHolder::Instance().update(now_ticks);
    city->blockAnims->update(now_ticks);
    if (!paused) {
      drawScene(now_ticks - last_tick);
    last_tick = now_ticks;
#ifdef WITH_LUA
      if (vm_tick_ok && (now_ticks - script_last_tick > 100)) {
        try {
          vm.callSimpleFunction("game_tick");
          script_last_tick = now_ticks;
        }
        catch (Exception & e) {
          vm_tick_ok = false;
          ERROR << "Disabling script game_tick because of error: " << e.what() << std::endl;
        }
      }
#endif
    }
    OpenGTA::SpriteManagerHolder::Instance().creationArea.setRects(
      city->getActiveRect(), city->getOnScreenRect());

//#ifdef TIMER_OPENSTEER_CLOCK
//    fps = int(timer.clock.getSmoothedFPS());
//#else
    if (now_ticks - fps_last_tick > 2000) {
      fps = num_frames_drawn / 2;
      num_frames_drawn = 0;
      fps_last_tick = now_ticks;
    }
//#endif
//    SDL_Delay(10);
  }

}
