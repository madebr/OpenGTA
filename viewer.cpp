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
#include <filesystem>
#include <iostream>
#include <iomanip>
#include <SDL_opengl.h>
#include <unistd.h>
#include "blockanim.h"
#include "config.h"
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
#include "gl_spritecache.h"
#include "timer.h"
#ifdef WITH_LUA
#include "lua_addon/lua_vm.h"
#include "lua_addon/lua_stackguard.h"
#endif
#include "gui.h"
#include "font_cache.h"
#include "ai.h"

#define getPedById getPed
#define removePedById removePed
#define addPed add

extern SDL_Surface* screen;
extern int global_EC;
extern int global_Done;
extern int global_Restart;
GLfloat mapPos[3] = {12.0f, 12.0f, 20.0f};

OpenGTA::CityView *city = NULL;
//OpenGL::DrawableFont* m_font = NULL;
GUI::Label * fps_label = NULL;

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
float anisotropic_filter_degree = 2.0f;
int mipmap_textures = -1;
int vsync_config = -1;
int city_blocks_area = -1;
int config_scale2x = 1;
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
bool gamma_slide = false;
float screen_gamma = 1.0f;

Vector3D test_dot(-1, -1, -1);

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
  //if (m_font)
  //  delete m_font;
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
  " -M k : texture mipmaps: 0 = disable, 1 = enable" << std::endl <<
  " -x k : scale2x sprites: 0 = disable, 1 = enable" << std::endl <<
  " -v k : vertical sync: 0 = disable, 1 = try with SDL" <<
#ifdef LINUX
  ", 2 = try with GLX" <<
#elif WIN32
  ", 2 = try with GLW" <<
#endif
    std::endl <<
  " -a f : anisotropic texture filtering degree: 1.0 = disabled" 
    << std::endl <<
  std::endl <<
  " -m map_file -g style_file : load specified files" << std::endl <<
  " -w width -h height        : screen dimension" << std::endl <<
  std::endl <<
  "City-num: 0 (default), 1, 2" << std::endl <<
  std::endl <<
  "The following environment variables are used when defined:" << std::endl <<
  " OGTA_DATA : PhysicsFS source for main data file lookup" << std::endl <<
  " OGTA_HOME : unused - will be config/save dir" << std::endl <<
  " OGTA_MOD  : PhysicsFS source to override main data files" << std::endl <<
  " OGTA_LANG : defines the fxt language file to load" << std::endl;
}

void print_version_info() {
#define PRINT_FORMATED(spaces) std::setw(spaces) << std::left <<
#define PRINT_OFFSET PRINT_FORMATED(19)
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
  PRINT_OFFSET "SDL_image support:" <<
#ifdef WITH_SDL_IMAGE
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
  PRINT_OFFSET "scale2x support:" <<
#ifdef DO_SCALE2X
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
#define VIEWER_FLAGS "a:s:w:h:c:m:M:g:l:v:x:fV"
#else
#define VIEWER_FLAGS "a:w:h:c:m:M:g:l:v:x:fV"
#endif
  while ((c = getopt (argc, argv, VIEWER_FLAGS)) != -1)
    switch (c)
    {
#ifdef WITH_LUA
      case 's':
        script_file = optarg;
        break;
#endif
      case 'a':
        anisotropic_filter_degree = atof(optarg);
        break;
      case 'c':
        highcolor_data = atoi(optarg); 
        break;
      case 'm':
        specific_map = std::string(optarg);
        break;
      case 'M':
        mipmap_textures = atoi(optarg);
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
      case 'v':
        vsync_config = atoi(optarg);
        break;
      case 'V':
        print_version_info();
        exit(0);
        break;
      case 'x':
        config_scale2x = atoi(optarg);
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
  const auto &data_path = Util::FileHelper::BaseDataPath();
  if (std::filesystem::exists(data_path))
    PHYSFS_mount(data_path.c_str(), nullptr, 1);
  else
    WARN << "Could not load data-source: " << data_path << std::endl;

  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);

  const auto &mod_path = Util::FileHelper::ModDataPath();
  if (std::filesystem::exists(mod_path))
    PHYSFS_mount(mod_path.c_str(), nullptr, 0);

  // screen, no window yet
  OpenGL::Screen & screen = OpenGL::Screen::Instance();

  // check for a configfile
#ifdef WITH_LUA
  if (PHYSFS_exists("config")) {
    char* config_as_string = (char*)Util::FileHelper::BufferFromVFS(
      Util::FileHelper::OpenReadVFS("config"));
    
    OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVM::Instance();
    try {
      //vm.runString(config_as_string);
      lua_State *L = vm.getInternalState();
      Util::LGUARD(L);
      if (luaL_loadbuffer(L, config_as_string, strlen(config_as_string), "config"))
        throw E_SCRIPTERROR("Error running string: " + std::string(lua_tostring(L, -1)));
      lua_newtable(L);
      lua_pushvalue(L, -1);
      lua_setglobal(L, "config");
      lua_setfenv(L, -2);
      if (lua_pcall(L, 0, 0, 0))
        throw E_SCRIPTERROR("Error running string: " + std::string(lua_tostring(L, -1)));
    }
    catch (const Util::ScriptError & e) {
      std::cerr << "Error in config-file: " << e.what() << std::endl;
      global_EC = 1;
      exit(1);
    }

    lua_State *L = vm.getInternalState();
    lua_getglobal(L, "config");
    if (lua_type(L, 1) == LUA_TTABLE) {
      try {
        bool sh = vm.getBool("use_g24_graphics");
        highcolor_data = sh;
      }
      catch (const Util::ScriptError & e) {}
      try {
        Uint32 sw = vm.getInt("screen_width");
        if (!arg_screen_w)
          arg_screen_w = sw;
      }
      catch (const Util::ScriptError & e) {}
      try {
        Uint32 sh = vm.getInt("screen_height");
        if (!arg_screen_h)
          arg_screen_h = sh;
      }
      catch (const Util::ScriptError & e) {}
      try {
        int sh = vm.getInt("screen_vsync");
        screen.setupVsync(sh);
      }
      catch (const Util::ScriptError & e) {}
      try {
        bool sh = vm.getBool("full_screen");
        if (!full_screen)
          full_screen = sh;
      }
      catch (const Util::ScriptError & e) {}

      float fov = screen.getFieldOfView();
      float np = screen.getNearPlane();
      float fp = screen.getFarPlane();
      try {
        fov = vm.getFloat("gl_field_of_view");
      }
      catch (const Util::ScriptError & e) {}
      try {
        np = vm.getFloat("gl_near_plane");
      }
      catch (const Util::ScriptError & e) {}
      try {
        fp = vm.getFloat("gl_far_plane");
      }
      catch (const Util::ScriptError & e) {}
      screen.setupGlVars(fov, np, fp);

      try {
        bool sh = vm.getBool("gl_mipmap_textures");
        ImageUtil::mipmapTextures = sh;
      }
      catch (const Util::ScriptError & e) {}

      try {
        bool sh = vm.getBool("scale2x_sprites");
        OpenGL::SpriteCache::Instance().setScale2x(sh);
      }
      catch (const Util::ScriptError & e) {}

      try {
        int sh = vm.getInt("active_area_size");
        city_blocks_area = sh;
      }
      catch (const Util::ScriptError & e) {}
    }
    // can't check for gl-extensions now

  }
#endif
  //INFO << "AREA:: " << city_blocks_area << std::endl;


  // check both width & height defined
  if ((arg_screen_h && !arg_screen_w) || (!arg_screen_h && arg_screen_w)) {
    WARN << "Invalid screen specified: " << arg_screen_w << "x" <<
      arg_screen_h << " - using default" << std::endl;
    arg_screen_h = 0; arg_screen_w = 0;
  }
  
  // fullscreen before first video init; only chance to set it on win32
  screen.setFullScreenFlag(full_screen);
  if (vsync_config != -1)
    screen.setupVsync(vsync_config);

  // create screen
  screen.activate(arg_screen_w, arg_screen_h);

  SDL_EnableKeyRepeat( 0, 0 ); 
  //SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );

  // more setup; that requires an active screen
#ifdef WITH_LUA
  OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVM::Instance();
  lua_State *L = vm.getInternalState();
  if (lua_type(L, 1) == LUA_TTABLE) {
    try {
      float v = vm.getFloat("gl_anisotropic_textures");
      if (ImageUtil::supportedMaxAnisoDegree >= v)
        ImageUtil::supportedMaxAnisoDegree = v;
    }
    catch (const Util::ScriptError & e) {}
    try {
      float v = (highcolor_data ? vm.getFloat("screen_gamma_g24") :
        vm.getFloat("screen_gamma_gry"));
      screen_gamma = v;
      SDL_SetGamma(v, v, v);
    }
    catch (const Util::ScriptError & e) {}
  }
  lua_settop(L, 0);
#endif
  if (ImageUtil::supportedMaxAnisoDegree >= anisotropic_filter_degree)
    ImageUtil::supportedMaxAnisoDegree = anisotropic_filter_degree;
  else
    WARN << "Using filter degree " << ImageUtil::supportedMaxAnisoDegree <<
    ", requested " << anisotropic_filter_degree << " not supported" << std::endl;

  switch(mipmap_textures) {
    case -1:
      break;
    case 0:
      ImageUtil::mipmapTextures = false;
      break;
    default:
      ImageUtil::mipmapTextures = true;
      break;
  }
  
  // before any graphics are loaded
  OpenGL::SpriteCache::Instance().setScale2x(config_scale2x);

  // FIXME: basic gui setup; should not be here
  GUI::Manager & gm = GUI::Manager::Instance();
  SDL_Rect rect;
  rect.x = 5;
  rect.y = 50;
  fps_label = new GUI::Label(rect, "", "F_MTEXT.FON", 1);
  //fps_label->borderColor.r = fps_label->borderColor.unused = 200;
  gm.add(fps_label, 5);
}


void print_position() {
  Vector3D & v = OpenGL::Camera::Instance().getCenter();
  Vector3D & e = OpenGL::Camera::Instance().getEye();
  Vector3D & u = OpenGL::Camera::Instance().getUp();
  if (!city->getViewMode()) {
  std::cout << cities[city_num] << ": " << city->getCurrentSector()->getFullName() << std::endl <<
  "camera.setCenter(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl <<
  "camera.setEye(" << e.x << ", " << e.y << ", " << e.z << ")" << std::endl <<
  "camera.setUp(" << u.x << ", " << u.y << ", " << u.z << ")" << std::endl <<
  "city_view:setVisibleRange(" << city->getVisibleRange() << ")" << std::endl <<
  "city_view:setTopDownView( false )" << std::endl;
  }
  else {
    GLfloat* cp = city->getCamPos();
    std::cout << cities[city_num] << ": " << city->getCurrentSector()->getFullName() << std::endl <<
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
  OpenGTA::Pedestrian p(Vector3D(0.2f, 0.5f, 0.2f), v, 0xffffffff);
  p.remap = OpenGTA::ActiveStyle::Instance().get().getRandomPedRemapNumber();
  INFO << "using remap: "  << p.remap << std::endl;
  OpenGTA::Pedestrian & pr = OpenGTA::SpriteManager::Instance().addPed(p);
  pr.switchToAnim(1);
  OpenGTA::LocalPlayer::Instance().setCtrl(pr.m_control);
  GUI::create_ingame_gui(1);
  //pr.m_control = &OpenGTA::LocalPlayer::Instance();
  //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).giveItem(1, 255);
}

void explode_ped() {
  try {
  OpenGTA::Pedestrian & ped = OpenGTA::SpriteManager::Instance().getPedById(0xffffffff);
  Vector3D p(ped.pos);
  p.y += 0.2f;
  OpenGTA::SpriteManager::Instance().createExplosion(p);
  }
  catch (Util::UnknownKey & e) {
    WARN << "Cannot place explosion - press F4 to switch to player-mode first!" << std::endl;
  }
}

void zoomToTrain(int k) {
/*
  OpenGTA::TrainSegment & ts = OpenGTA::SpriteManager::Instance().getTrainById(k);
  Vector3D p(ts.pos);
  p.y += 9;
  OpenGL::Camera::Instance().interpolate(p, 1, 30000);
*/

}

#include "cell_iterator.h"

namespace OpenGTA {
  void ai_step_fake(OpenGTA::Pedestrian *p) {
    try {
    OpenGTA::Pedestrian & pr = OpenGTA::SpriteManager::Instance().getPedById(0xffffffff);
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
  OpenGTA::Pedestrian & pr = OpenGTA::SpriteManager::Instance().getPedById(0xffffffff);
  int id = OpenGTA::TypeIdBlackBox::requestId();
  Vector3D v(pr.pos);
  v.y += 0.9f;
  //INFO << v.x << " " << v.y << " " << v.z << std::endl;
  Sint16 remap = OpenGTA::ActiveStyle::Instance().get().getRandomPedRemapNumber();
  OpenGTA::Pedestrian p(Vector3D(0.2f, 0.5f, 0.2f), v, id, remap);
  OpenGTA::SpriteManager::Instance().addPed(p);
  OpenGTA::Pedestrian & pr2 = OpenGTA::SpriteManager::Instance().getPedById(id);
  pr2.switchToAnim(1);
  INFO << "now " << OpenGTA::SpriteManager::Instance().getNum<OpenGTA::Pedestrian>() << " peds " << std::endl;

  //pr2.m_control = &OpenGTA::nullAI;
  }
  catch (Util::UnknownKey & e) {
    WARN << "Cannot place peds now - press F4 to switch to player-mode first!" << std::endl;
  }
}

void toggle_player_run() {
  OpenGTA::PedController * pc = &OpenGTA::LocalPlayer::Instance().getCtrl();
  INFO << std::endl;

  if (!pc) {
    WARN << "no player yet!" << std::endl;
    return;
  }
  if (!pc->getRunning()) 
    pc->setRunning(true);
  else
    pc->setRunning(false);
}

void show_gamma_config() {
  OpenGL::Screen & screen = OpenGL::Screen::Instance();
  GUI::Manager & gm = GUI::Manager::Instance();
  if (gamma_slide) {
  SDL_Rect r;

  r.x = screen.getWidth() / 2;
  r.y = screen.getHeight() / 2;
  r.w = 200;
  r.h = 30;

  GUI::ScrollBar * sb = new GUI::ScrollBar(GUI::GAMMA_SCROLLBAR_ID, r);
  sb->color.r = sb->color.g = sb->color.b = 180;
  sb->color.unused = 255;
  sb->innerColor.r = 250;
  sb->value = screen_gamma/2;
  sb->changeCB = GUI::ScrollBar::SC_Functor(GUI::screen_gamma_callback);
  gm.add(sb, 90);

  r.y += 40;
  GUI::Label *l = new GUI::Label(GUI::GAMMA_LABEL_ID,
                                 r,
                                 "Gamma: " + std::to_string(screen_gamma),
                                 "F_MTEXT.FON",
                                 1);
  gm.add(l, 80);

  screen.setSystemMouseCursor(true);

  }
  else {
    gm.removeById(GUI::GAMMA_SCROLLBAR_ID);
    gm.removeById(GUI::GAMMA_LABEL_ID);
    screen.setSystemMouseCursor(false);
  }
}

void car_toggle() {
  OpenGTA::Pedestrian & pped = OpenGTA::LocalPlayer::Instance().getPed();
  Vector3D pos = pped.pos;
  std::list<OpenGTA::Car> & list = OpenGTA::SpriteManager::Instance().getList<OpenGTA::Car>();
  float min_dist = 360;
  float _d;
  std::list<OpenGTA::Car>::iterator j = list.end();
  for (std::list<OpenGTA::Car>::iterator i = list.begin(); i != list.end(); i++) {
    if ((_d = Util::distance(pos, i->pos)) < min_dist) {
      j = i;
      min_dist = _d; 
    }
  }
  assert(j != list.end());
  std::cout << j->id() << " " << j->pos.x << ", " << j->pos.y << ", " << j->pos.z << std::endl;
  Vector3D p_door(j->carInfo.door[0].rpx / 64.0f, 0, 
    j->carInfo.door[0].rpy / 64.0f);

  Vector3D p_door_global = Transform(p_door, j->m_M);
  p_door_global.y += 0.2f;
  std::cout << p_door_global.x << ", " << p_door_global.y << ", " << p_door_global.z << std::endl;
  test_dot = p_door_global;
  //pped.aiMode = 1;
  //pped.aiData.pos1 = p_door_global;
  OpenGTA::AI::Pedestrian::walk_pavement(&pped);

}

void handleKeyPress( SDL_keysym *keysym ) {
  GLfloat* cp = city->getCamPos();
  mapPos[0] = cp[0]; mapPos[1] = cp[1]; mapPos[2] = cp[2];
  OpenGL::Camera & cam = OpenGL::Camera::Instance();
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
      //if (next_station_zoom >= OpenGTA::SpriteManager::Instance().trainSystem.getNumTrains())
      //  next_station_zoom = 0;
      global_Restart = 1;
      global_Done = 1;
      return;
      {
        Vector3D p(cam.getEye());
        OpenGTA::ActiveMap::Instance().get().getNearestLocationByType(0, p.x, p.z);
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
    case SDLK_F4:
      follow_toggle = (follow_toggle ? 0 : 1);
      if (follow_toggle) {
        SDL_EnableKeyRepeat( 0, SDL_DEFAULT_REPEAT_INTERVAL );
        city->setViewMode(false);
        Vector3D p(cam.getEye());
        create_ped_at(p);
        cam.setVectors( Vector3D(p.x, 10, p.z), Vector3D(p.x, 9.0f, p.z), Vector3D(0, 0, -1) );
        cam.setFollowMode(OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).pos);
        cam.setCamGravity(true);
      }
      else {
        SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
        cam.setVectors(cam.getEye(), 
          Vector3D(cam.getEye() + Vector3D(1, -1, 1)), Vector3D(0, 1, 0));
        cam.setCamGravity(false);
        cam.releaseFollowMode();
        OpenGTA::SpriteManager::Instance().removePedById(0xffffffff);
        OpenGTA::SpriteManager::Instance().removeDeadStuff();
        GUI::remove_ingame_gui();
      }
      break;
    case SDLK_RETURN:
      car_toggle();
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
    case SDLK_F12:
      gamma_slide = (gamma_slide ? 0 : 1);
      show_gamma_config();
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
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(1);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(1);
      break;
    case '2':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(2);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(2);
      break;
    case '3':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(3);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(3);
      break;
    case '4':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(4);
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(4);
      break;
    case '5':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(5);
      break;
    case '6':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(6);
      break;
    case '7':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(7);
      break;
    case '8':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(8);
      break;
    case '9':
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(9);
      /*
      ped_anim -= 1; if (ped_anim < 0) ped_anim = 0;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      break;
    case '0':
      OpenGTA::LocalPlayer::Instance().getCtrl().setActiveWeapon(0);
      /*
      ped_anim += 1; if (ped_anim > 200) ped_anim = 200;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      //OpenGTA::SpriteManager::Instance().getPedById(0xffffffff).equip(0);
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
//FIXME: simply ignored on windows for now
#ifndef WIN32
      OpenGL::Screen::Instance().toggleFullscreen();
#endif
#if 0
#ifdef WIN32
      city->resetTextures();
      //m_font->resetTextures();
      OpenGL::SpriteCache::Instance().clearAll();
#endif
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
      OpenGL::Screen::Instance().makeScreenshot("screenshot.bmp"); 
      break;
    default:
      return; 
  }
  city->setPosition(mapPos[0], mapPos[1], mapPos[2]);

}

void drawScene(Uint32 ticks) {
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  OpenGL::Screen::Instance().set3DProjection();
  city->draw(ticks);

  glColor3f(1, 0, 0);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_POINTS);
  glVertex3f(test_dot.x, test_dot.y, test_dot.z);
  glEnd();
  glEnable(GL_TEXTURE_2D);
  glColor3f(1, 1, 1);


  OpenGL::Screen::Instance().setFlatProjection();
  glDisable(GL_DEPTH_TEST);
  
  glPushMatrix();
  glTranslatef(10, 10, 0);
  OpenGL::DrawableFont & m_font = OpenGTA::FontCache::Instance().getFont("F_MTEXT.FON", 1);
  m_font.drawString(city->getCurrentSector()->getFullName());
  glPopMatrix();
  
  /*glPushMatrix();
  glTranslatef(5, 50, 0);
  std::ostringstream strstr;
  strstr << fps << " fps";
  m_font->drawString(strstr.str());
  glPopMatrix();*/

  GUI::Manager::Instance().draw();
 
  num_frames_drawn += 1;
  glEnable(GL_DEPTH_TEST);

  SDL_GL_SwapBuffers();
}

void draw_mapmode() {
  SDL_Event event;
  OpenGL::PagedTexture map_tex = city->renderMap2Texture();
  bool done_map = false;
  OpenGL::Screen & screen = OpenGL::Screen::Instance();
  screen.setSystemMouseCursor(true);
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);
  INFO << map_tex.coords[1].u << std::endl;
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
            default:
              break;
          }
          break;
        case SDL_MOUSEBUTTONDOWN:
          INFO << event.button.x  / 600.0f * 255<< " " << event.button.y / 600.0f * 255 << std::endl;
          mapPos[0] = event.button.x  / 600.0f * 255;
          mapPos[2] = event.button.y / 600.0f * 255;
          //mapPos[1] = 10;
          done_map = true;
          break;
        case SDL_MOUSEMOTION:
          std::cout << "Mouse move: x " << event.motion.x << " y " << 
            event.motion.y << std::endl;
          break;
        default:
          break;
      }
    }

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    screen.setFlatProjection();
    glBindTexture(GL_TEXTURE_2D, map_tex.inPage);
    //glMatrixMode(GL_TEXTURE);
    //if (_scale < 1)
    //  glScalef(_scale, _scale, 1);


    uint32_t h = screen.getHeight();
    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2i(0, 0);
    glTexCoord2f(map_tex.coords[1].u, 0);
    glVertex2i(h, 0);
    glTexCoord2f(map_tex.coords[1].u, map_tex.coords[1].v);
    glVertex2i(h, h);
    glTexCoord2f(0, map_tex.coords[1].v);
    glVertex2i(0, h);
    glEnd();

    const OpenGTA::Map::LocationMap & lmap = OpenGTA::ActiveMap::Instance().get().getLocationMap();
    OpenGTA::Map::LocationMap::const_iterator i = lmap.begin();
    glDisable(GL_TEXTURE_2D);
    while (i != lmap.end()) {
      if (i->first == 2) {
        i++;
        continue;
      }
      // uint8_t l_type = i->first;
      float l_x, l_y;
      l_x = i->second->x / 255.0f * h;// *  map_tex.coords[1].u;
      l_y = i->second->y / 255.0f * h;// * map_tex.coords[1].u;
      //INFO << int(l_type) << ": " << l_x << " " << l_y << std::endl;
      glBegin(GL_LINE_STRIP);
      glVertex2f(l_x - 5, l_y - 5);
      glVertex2f(l_x + 5, l_y - 5);
      glVertex2f(l_x + 5, l_y + 5);
      glVertex2f(l_x - 5, l_y + 5);

      glEnd();
      ++i;
    }
    
    glEnable(GL_TEXTURE_2D);
    SDL_GL_SwapBuffers();
    SDL_Delay(20);

  }
  screen.setSystemMouseCursor(false);
  glEnable(GL_DEPTH_TEST);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  // the texture class doesn't cleanup!
  glDeleteTextures(1, &map_tex.inPage); 
}

void run_main() {
  SDL_Event event;
  const char * lang = getenv("OGTA_LANG");
  if (!lang)
    lang = getenv("LANG");
  if (!lang)
    lang = "en";
  OpenGTA::MainMsgLookup::Instance().load(Util::FileHelper::Lang2MsgFilename(lang));

  //m_font = new OpenGL::DrawableFont();
  //m_font->loadFont("F_MTEXT.FON");
  //m_font->setScale(1);

  glEnable(GL_TEXTURE_2D);
  glPolygonMode(GL_FRONT, GL_FILL);
  glEnable(GL_CULL_FACE);

  //glEnable(GL_BLEND);
  //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 2/255.0f);//0);

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
  if (city_blocks_area > -1)
    city->setVisibleRange(city_blocks_area);
  city->setPosition(mapPos[0], mapPos[1], mapPos[2]);

  OpenGL::Camera & cam = OpenGL::Camera::Instance(); 
  //cam.setVectors( Vector3D(4, 10, 4), Vector3D(4, 0.0f, 4.0f), Vector3D(0, 0, -1) );
  cam.setVectors( Vector3D(12, 20, 12), Vector3D(13.0f, 19.0f, 13.0f), Vector3D(0, 1, 0) );

#ifdef TIMER_OPENSTEER_CLOCK
  Timer & timer = Timer::Instance();
  timer.update();
  last_tick = timer.getRealTime();
  //timer.setSimulationRunning(true);
#else
  last_tick = SDL_GetTicks();
#endif

#ifdef WITH_LUA
  OpenGTA::Script::LuaVM & vm = OpenGTA::Script::LuaVM::Instance();
  vm.setCityView(*city);
  vm.setMap(OpenGTA::ActiveMap::Instance().get());
  if (script_file)
    vm.runFile(script_file);
  bool vm_tick_ok = true;
  script_last_tick = last_tick;
#endif
  
  GUI::Manager & guiManager = GUI::Manager::Instance();

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
          OpenGL::Screen::Instance().resize(event.resize.w, event.resize.h);
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        case SDL_MOUSEMOTION:
          //std::cout << "Mouse move: x " << float(event.motion.x)/screen->w << " y " << float(event.motion.y)/screen->h << std::endl;
          break;
        case SDL_MOUSEBUTTONDOWN:
          guiManager.receive(event.button);
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
    OpenGTA::SpriteManager::Instance().update(now_ticks);
    city->blockAnims->update(now_ticks);
    GUI::Manager::Instance().update(now_ticks);
    GUI::update_ingame_gui_values();
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
    OpenGTA::SpriteManager::Instance().creationArea.setRects(
      city->getActiveRect(), city->getOnScreenRect());

//#ifdef TIMER_OPENSTEER_CLOCK
//    fps = int(timer.clock.getSmoothedFPS());
//#else
    if (now_ticks - fps_last_tick > 2000) {
      fps = num_frames_drawn / 2;
      num_frames_drawn = 0;
      fps_last_tick = now_ticks;
      fps_label->text = std::to_string(uint32_t(fps)) + " fps";
#ifdef WITH_LUA
      vm.setGlobalInt("current_fps", fps);
#endif
    }
//#endif
//    SDL_Delay(10);
  }

#ifdef WITH_LUA
  vm.runFile("scripts/dump_config.lua");
#endif

}
