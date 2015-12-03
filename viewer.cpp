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
#include <iostream>
#include <sstream>
#include <SDL_opengl.h>
#include <unistd.h>
//#include "common_sdl_gl.h"
#include "gl_screen.h"
#include "opengta.h"
#include "gl_texturecache.h"
#include "gl_cityview.h"
#include "gl_font.h"
#include "gl_camera.h"
#include "navdata.h"
#include "log.h"
#include "spritemanager.h"
#include "localplayer.h"
#include "m_exceptions.h"
#include "file_helper.h"
#include "gl_spritecache.h"
#ifdef WITH_LUA
#include "lua_addon/lua_vm.h"
#endif

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
bool highcolor_data = false;
bool full_screen = false;
bool player_toggle_run = false;

const char* script_file = NULL;
int paused = 0;

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
      OpenGTA::LocalPlayer::Instance().turn = 0;
      break;
    case 'l':
      OpenGTA::LocalPlayer::Instance().turn = 0;
      break;
    case 'i':
      OpenGTA::LocalPlayer::Instance().move = 0;
      break;
    case 'k':
      OpenGTA::LocalPlayer::Instance().move = 0;
      break;
    default:
      break;
  }
}

void draw_mapmode();

void create_ped_at(const Vector3D v) {
  OpenGTA::Pedestrian p(Vector3D(0.3f, 0.5f, 0.3f), v, 0xffffffff);
  p.m_control = &OpenGTA::LocalPlayer::Instance();
  OpenGTA::SpriteManagerHolder::Instance().addPed(p);
  OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).switchToAnim(1);
  OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).giveItem(1, 255);
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
      cam.interpolate(Vector3D(254, 9, 254), 1, 20000);
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
        city->setViewMode(false);
        Vector3D p(cam.getEye());
        create_ped_at(p);
        cam.setVectors( Vector3D(p.x, 10, p.z), Vector3D(p.x, 0.0f, p.z), Vector3D(0, 0, -1) );
        cam.setFollowMode(OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).pos);
      }
      else {
        cam.setVectors(cam.getEye(), 
          Vector3D(cam.getEye() + Vector3D(1, -1, 1)), Vector3D(0, 1, 0));
        cam.releaseFollowMode();
        OpenGTA::SpriteManagerHolder::Instance().removePedById(0xffffffff);
      }
      break;
    case SDLK_F5:
      draw_arrows = (draw_arrows ? 0 : 1);
      city->setDrawHeadingArrows(draw_arrows);
      break;
    case SDLK_F6:
      draw_mapmode();
      break;
    case SDLK_F9:
      city->setDrawTextured(city->getDrawTextured() ? 0 : 1);
      break;
    case SDLK_F10:
      city->setDrawLines(city->getDrawLines() ? 0 : 1);
      break;
    /*
    case SDLK_F6:
      tex_flip = (tex_flip ? 0 : 1);
      INFO << "flipping: " << tex_flip << std::endl;
      city->setTexFlipTest(tex_flip);
      break;
    */
    case '1':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(1);
      break;
    case '2':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(2);
      break;
    case '3':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(3);
      break;
    case '4':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(4);
      break;
    case '5':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(5);
      break;
    case '6':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(6);
      break;
    case '7':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(7);
      break;
    case '8':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(8);
      break;
    case '9':
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(9);
      /*
      ped_anim -= 1; if (ped_anim < 0) ped_anim = 0;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      break;
    case '0':
      /*
      ped_anim += 1; if (ped_anim > 200) ped_anim = 200;
      pedAnim.firstFrameOffset = ped_anim;
      INFO << "switching to sprite: " << ped_anim << std::endl;
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).setAnimation(pedAnim);
      */
      OpenGTA::SpriteManagerHolder::Instance().getPedById(0xffffffff).equip(0);
      break;
    case 'w':
      cam.setSpeed(0.2f);
      break;
    case 's':
      cam.setSpeed(-0.2f);
      break;
    case 'j':
      OpenGTA::LocalPlayer::Instance().turn = 1;
      break;
    case 'l':
      OpenGTA::LocalPlayer::Instance().turn = -1;
      break;
    case 'i':
      OpenGTA::LocalPlayer::Instance().move = (player_toggle_run) ? 2 : 1;
      break;
    case 'k':
      OpenGTA::LocalPlayer::Instance().move = -1;
      break;
    case SDLK_LSHIFT:
      player_toggle_run = (player_toggle_run) ? false : true;
      INFO << player_toggle_run << std::endl;
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
  glTranslatef(5, 50, 0);
  std::ostringstream strstr;
  strstr << fps << " fps";
  m_font->drawString(strstr.str());

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

void run_init() {
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
  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );
}

void run_main() {
  SDL_Event event;

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

  last_tick = SDL_GetTicks();
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
    Uint32 now_ticks = SDL_GetTicks();
    OpenGTA::SpriteManagerHolder::Instance().update(now_ticks);
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
    if (now_ticks - fps_last_tick > 2000) {
      fps = num_frames_drawn / 2;
      num_frames_drawn = 0;
      fps_last_tick = now_ticks;
    }
//    SDL_Delay(10);
  }
}
