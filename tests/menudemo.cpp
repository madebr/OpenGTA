#include <iostream>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <unistd.h>
#include <physfs.h>
#include "gl_screen.h"
#include "gl_pagedtexture.h"
#include "log.h"
#include "buffercache.h"
#include "physfsrwops.h"
#include "m_exceptions.h"
#include "gui.h"
#include "timer.h"
#include "dataholder.h"
#include "gl_spritecache.h"
#include "m_exceptions.h"

extern int global_EC;
extern int global_Done;

Uint32 arg_screen_w = 800;
Uint32 arg_screen_h = 600;

namespace GUI {
  Object::Object(const SDL_Rect & r) :
    id(0), rect(), color(),
    manager(Manager::Instance()) {
    copyRect(r);
    color.r = 255; color.g = 255; color.b = 255; color.unused = 255;
  }
  Object::Object(const size_t Id, const SDL_Rect & r) :
    id(Id), rect(), color(),
    manager(Manager::Instance()) {
    copyRect(r);
    color.r = 255; color.g = 255; color.b = 255; color.unused = 255;
  }
  Object::Object(const size_t Id, const SDL_Rect & r, const SDL_Color & c) :
    id(Id), rect(), color(),
    manager(Manager::Instance()) {
    copyRect(r);
    copyColor(c);
  }
  void Object::draw() {
    glColor4ub(color.r, color.g, color.b, color.unused);
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + rect.w, rect.y);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
  }
  void Object::copyRect(const SDL_Rect & src) {
    rect.x = src.x;
    rect.y = src.y;
    rect.w = src.w;
    rect.h = src.h;
  }
  void Object::copyColor(const SDL_Color & src) {
    color.r = src.r;
    color.g = src.g;
    color.b = src.b;
    color.unused = src.unused;
  }

  void TexturedObject::draw() {

    const OpenGL::PagedTexture & tex = manager.getCachedImage(texId);
    glColor4ub(color.r, color.g, color.b, color.unused);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.inPage);
    glBegin(GL_QUADS);
    glTexCoord2f(tex.coords[0].u, tex.coords[1].v);
    glVertex2i(rect.x, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[1].v);
    glVertex2i(rect.x + rect.w, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[0].v);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glTexCoord2f(tex.coords[0].u, tex.coords[0].v);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }

  void AnimatedTextureObject::draw() {
    if (!animation)
      animation = manager.findAnimation(animId);

    assert(animation);
    size_t texId = animation->getCurrentFrame();
    const OpenGL::PagedTexture & tex = manager.getCachedImage(texId);
    glColor4ub(color.r, color.g, color.b, color.unused);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.inPage);
    glBegin(GL_QUADS);
    glTexCoord2f(tex.coords[0].u, tex.coords[1].v);
    glVertex2i(rect.x, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[1].v);
    glVertex2i(rect.x + rect.w, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[0].v);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glTexCoord2f(tex.coords[0].u, tex.coords[0].v);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
    glDisable(GL_TEXTURE_2D);
  }

  void Label::draw() {
    glPushMatrix();
    glColor4ub(color.r, color.g, color.b, color.unused);
    glEnable(GL_TEXTURE_2D);
    glTranslatef(rect.x, rect.y, 0);
    rect.w = uint16_t(font->drawString(text));
    rect.h = font->getHeight();
    glPopMatrix();
    /*
       glDisable(GL_TEXTURE_2D);
       glBegin(GL_LINE_STRIP);
       glVertex2i(rect.x, rect.y);
       glVertex2i(rect.x + rect.w, rect.y);
       glVertex2i(rect.x + rect.w, rect.y + rect.h);
       glVertex2i(rect.x, rect.y + rect.h);
       glVertex2i(rect.x, rect.y);
       glEnd();
       */
  }

  void Pager::draw() {

    const OpenGL::PagedTexture & tex = manager.getCachedImage(texId);
    glColor4ub(color.r, color.g, color.b, color.unused);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex.inPage);
    glBegin(GL_QUADS);
    glTexCoord2f(tex.coords[0].u, tex.coords[1].v);
    glVertex2i(rect.x, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[1].v);
    glVertex2i(rect.x + rect.w, rect.y);
    glTexCoord2f(tex.coords[1].u, tex.coords[0].v);
    glVertex2i(rect.x + rect.w, rect.y + rect.h);
    glTexCoord2f(tex.coords[0].u, tex.coords[0].v);
    glVertex2i(rect.x, rect.y + rect.h);
    glEnd();
    glScissor(rect.x, rect.y, rect.w, rect.h);
    glEnable(GL_SCISSOR_TEST);
    
    glPushMatrix();
    glColor4ub(color.r, color.g, color.b, color.unused);
    glTranslatef(rect.x+offset, rect.y+4, 0);
    int slen = (int)font->drawString("test - hello world, this is a really long message. it will not end. never... or maybe sometime, but not yet. The end.");
    if (slen + offset < -10) {
      color.unused = 0;
      manager.remove(this);
    }
    INFO << slen <<" " << offset << std::endl;
    glPopMatrix();

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_TEXTURE_2D);

  }

  void Pager::update(Uint32 ticks) {
    offset -= 1;

    //if (offset < -50)
    //  color.unused = 0;
  }

  void Manager::draw() {
    GuiObjectListMap::iterator layer_it = guiLayers.begin();

    while (layer_it != guiLayers.end()) {
      GuiObjectList & inThisLayer = layer_it->second;
      for (GuiObjectList::iterator obj_it = inThisLayer.begin();
          obj_it != inThisLayer.end(); ++obj_it) {
        Object * obj = *obj_it;
        obj->draw();
      }
      ++layer_it;
    }
  }

  Manager::~Manager() {
    clearObjects();
    clearCache();
  }

  void Manager::clearObjects() {
    INFO << "clearing gui objects" << std::endl;
    GuiObjectListMap::iterator layer_it = guiLayers.begin();
    while (layer_it != guiLayers.end()) {
      GuiObjectList & list = layer_it->second;
      for (GuiObjectList::iterator i = list.begin(); i != list.end(); ++i) {
        delete *i;
      }
      list.clear();
      ++layer_it;
    }
    guiLayers.clear();
  }

  void Manager::clearCache() {
    INFO << "clearing gui texture cache" << std::endl;
    for (GuiTextureCache::iterator i = texCache.begin(); i != texCache.end(); ++i) {
      OpenGL::PagedTexture & p = i->second;
      glDeleteTextures(1, &p.inPage);
    }
    texCache.clear();
    for (AnimationMap::iterator i = guiAnimations.begin(); i != guiAnimations.end(); ++i) {
      delete i->second;
    }
    guiAnimations.clear();
  }

  const OpenGL::PagedTexture & Manager::getCachedImage(size_t Id) {
    GuiTextureCache::iterator i = findByCacheId(Id);
    if (i == texCache.end())
      throw E_UNKNOWNKEY("cached texture id " + std::to_string(int(Id)));
    return i->second;
  }

  Manager::GuiTextureCache::iterator Manager::findByCacheId(const size_t & Id) {
    return texCache.find(Id);
  }

  Manager::GuiObjectListMap::iterator Manager::findLayer(uint8_t l) {
    return guiLayers.find(l);
  }

  void Manager::cacheImageRAW(const std::string & file, size_t k) {
    texCache.insert(std::make_pair<size_t, OpenGL::PagedTexture>(k, ImageUtil::loadImageRAW(file)));
  }

  void Manager::cacheImageRAT(const std::string & file, const std::string & palette, size_t k) {
    texCache.insert(std::make_pair<size_t, OpenGL::PagedTexture>(k, 
          ImageUtil::loadImageRATWithPalette(file, palette)));
  }

  // FIXME: move stuff to ImageUtil
  void Manager::cacheImageSDL(const std::string & file, size_t k) {
    SDL_RWops * rwops = PHYSFSRWOPS_openRead(file.c_str());
    //SDL_Surface *surface = IMG_Load(file.c_str());
    SDL_Surface *surface = IMG_Load_RW(rwops, 1);
    assert(surface);

    ImageUtil::NextPowerOfTwo npot(surface->w, surface->h);
    uint16_t bpp = surface->format->BytesPerPixel;

    uint8_t * buffer = Util::BufferCache::Instance().requestBuffer(npot.w * npot.h * bpp);
    SDL_LockSurface(surface);
    ImageUtil::copyImage2Image(buffer, (uint8_t*)surface->pixels, surface->pitch, surface->h,
        npot.w * bpp);
    SDL_UnlockSurface(surface);

    GLuint texture = ImageUtil::createGLTexture(npot.w, npot.h, (bpp == 4) ? true : false, buffer);
    texCache.insert(std::make_pair<size_t, OpenGL::PagedTexture>(k, 
          OpenGL::PagedTexture(texture, 0, 0, GLfloat(surface->w)/npot.w, GLfloat(surface->h)/npot.h)));
    SDL_FreeSurface(surface);
  }

  ImageUtil::WidthHeightPair Manager::cacheStyleArrowSprite(const size_t id, int remap) {
    OpenGTA::GraphicsBase & graphics = OpenGTA::ActiveStyle::Instance().get();
    PHYSFS_uint16 t = graphics.spriteNumbers.reIndex(id, OpenGTA::GraphicsBase::SpriteNumbers::ARROW);
    OpenGTA::GraphicsBase::SpriteInfo * info = graphics.getSprite(t);
    texCache.insert(std::make_pair<size_t, OpenGL::PagedTexture>(
      id, OpenGL::SpriteCache::Instance().createSprite(size_t(t), remap, 0, info)
    ));
    return ImageUtil::WidthHeightPair(info->w, info->h);
  }

  void Manager::add(Object * obj, uint8_t onLevel) {
    GuiObjectListMap::iterator l = findLayer(onLevel);
    if (l == guiLayers.end()) {
      GuiObjectList list;
      list.push_back(obj);
      guiLayers.insert(std::make_pair<uint8_t, GuiObjectList>(onLevel, list));
      return;
    }
    GuiObjectList & list = l->second;
    list.push_back(obj);
  }

  void Manager::remove(Object * obj) {
    for (GuiObjectListMap::iterator l = guiLayers.begin(); l != guiLayers.end(); ++l) {
      GuiObjectList & list = l->second;
      for (GuiObjectList::iterator m = list.begin(); m != list.end(); ++m) {
        Object * o = *m;
        if (o == obj) {
          delete o;
          list.erase(m);
          return;
        }
      }
    }
    throw E_UNKNOWNKEY("not a managed object-ptr");
  }

  Object* Manager::findObject(const size_t id) {
    for (GuiObjectListMap::iterator l = guiLayers.begin(); l != guiLayers.end(); ++l) {
      GuiObjectList & list = l->second;
      for (GuiObjectList::iterator m = list.begin(); m != list.end(); ++m) {
        Object * o = *m;
        if (o->id == id)
          return o;
      }
    }
    throw E_UNKNOWNKEY("object by id " + std::to_string(int(id)));
  }

  void Manager::removeById(const size_t id) {
    Object * o = findObject(id);
    if (o)
      remove(o);
    //else
    //  ERROR << "failed to find object " << id << " - cannot remove it" << std::endl;
  }

  bool Manager::isInside(Object & obj, Uint16 x, Uint16 y) const {
    if ((obj.rect.x <= x) && (x <= obj.rect.x + obj.rect.w) &&
        (obj.rect.y <= y) && (y <= obj.rect.y + obj.rect.h))
      return true;
    return false;
  }

  void Manager::receive(SDL_MouseButtonEvent & mb_event) {
    Uint32 sh = OpenGL::Screen::Instance().getHeight();
    GuiObjectListMap::reverse_iterator l = guiLayers.rbegin();
    while (l != guiLayers.rend()) {
      std::cout << int(l->first) << std::endl;
      GuiObjectList & list = l->second;
      for (GuiObjectList::iterator i = list.begin(); i != list.end(); ++i) {
        Object * obj = *i;
        if (isInside(*obj, mb_event.x, sh - mb_event.y)) {
          std::cout << "mouse inside obj id: " << obj->id << " at " << mb_event.x << "," << mb_event.y << std::endl;
          return;
        }
      }
      ++l;
    }
  }

  void Manager::update(uint32_t nowticks) {
    for (AnimationMap::iterator i = guiAnimations.begin(); i != guiAnimations.end(); ++i) {
      i->second->update(nowticks);
    }
    GuiObjectListMap::iterator layer_it = guiLayers.begin();
    while (layer_it != guiLayers.end()) {
      GuiObjectList & inThisLayer = layer_it->second;
      for (GuiObjectList::iterator obj_it = inThisLayer.begin();
          obj_it != inThisLayer.end(); ++obj_it) {
        Object * obj = *obj_it;
        obj->update(nowticks);
      }
      ++layer_it;
    }
  }

  Animation* Manager::findAnimation(uint16_t id) {
    AnimationMap::iterator i = guiAnimations.find(id);
    return i->second;
  }

  void Manager::createAnimation(const std::vector<uint16_t> & indices, uint16_t fps, size_t k) {
    Animation * anim = new Animation(indices, fps);
    guiAnimations.insert(std::make_pair<size_t, Animation*>(k, anim));
    anim->set(Util::Animation::PLAY_FORWARD, Util::Animation::LOOP);
  }

  uint16_t Animation::getCurrentFrame() {
    return indices[getCurrentFrameNumber()];
  }

}


#ifdef GUI_MENU_DEMO
GUI::Manager guiManager;

void on_exit() {
  SDL_Quit();
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
  else
    std::cout << "Goodbye" << std::endl;
}

void parse_args(int argc, char* argv[]) {
}

void turn_anim_off(float) {
  GUI::AnimatedTextureObject * obj= (GUI::AnimatedTextureObject *)guiManager.findObject(2);
  obj->animation->set(obj->animation->get(), Util::Animation::STOP);
  INFO << "stopped animation" << std::endl;
}

void font_play(float b) {
  //INFO << b << std::endl;
  GUI::Object * obj = guiManager.findObject(5);
  obj->color.r = obj->color.g = obj->color.b = obj->color.unused = Uint8((1.0f - b) * 255);
}

void run_init() {
  PHYSFS_init("ogta");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
  if (getenv("OGTA_MOD"))
    PHYSFS_mount(getenv("OGTA_MOD"), nullptr, 0);
  OpenGL::Screen & screen = OpenGL::Screen::Instance();
  screen.activate(arg_screen_w, arg_screen_h);
  screen.setSystemMouseCursor(true);
  glEnable(GL_ALPHA_TEST);
  glAlphaFunc(GL_GREATER, 0);

  OpenGTA::ActiveStyle::Instance().load("STYLE001.G24");

  SDL_EnableKeyRepeat( 100, SDL_DEFAULT_REPEAT_INTERVAL );

  std::vector<uint16_t> frame_nums(8);
  for (int i = 0; i < 8; ++i) {
    guiManager.cacheImageRAW("F_LOGO" + std::to_string(i) + ".RAW", 100+i);
    frame_nums[i] = 100+i;
  }
  guiManager.createAnimation(frame_nums, 5, 1);

  SDL_Rect r;
  r.x = 0; r.y = 0; r.h = 312 * screen.getHeight() / 480; r.w = screen.getWidth();
  std::string rawfile("F_LOWER1.RAW");
  guiManager.cacheImageRAW(rawfile, 99);
  GUI::TexturedObject * b2 = new GUI::TexturedObject(1, r, 99);
  guiManager.add(b2, 2);
  r.y = 312 * screen.getHeight() / 480;
  r.h = 168 * screen.getHeight() / 480;
  GUI::AnimatedTextureObject * b3 = new GUI::AnimatedTextureObject(2, r, 1);
  guiManager.add(b3, 2);

  Timer & t = Timer::Instance();


  ImageUtil::WidthHeightPair whp = guiManager.cacheStyleArrowSprite(16, -1);
  guiManager.cacheStyleArrowSprite(17, -1);
  std::vector<uint16_t> anim2frames(2);
  anim2frames[0] = 16;
  anim2frames[1] = 17;
  guiManager.createAnimation(anim2frames, 10, 2);
  r.x = 200;
  r.y = 200;
  r.w = whp.first * 2; r.h = whp.second * 2;
  GUI::AnimatedTextureObject * b5 = new GUI::AnimatedTextureObject(1, r, 2);
  guiManager.add(b5, 3);

  whp = guiManager.cacheStyleArrowSprite(2, -1);
  r.w = whp.first;
  r.h = whp.second;
  GUI::Pager * pager = new GUI::Pager(3, r, 2, "STREET1.FON", 1);

  guiManager.add(pager, 5);

  /*Timer::CallbackType cmd(turn_anim_off);
  t.registerCallback(false, cmd, t.getRealTime() + 2000);

  Timer::CallbackType cmd2(font_play);
  t.registerCallback(false, cmd2, t.getRealTime() + 100, t.getRealTime() + 3000);
  */
}

void handleKeyPress( SDL_keysym *keysym ) {
  switch ( keysym->sym ) {
    case SDLK_ESCAPE:
      global_Done = 1;
      break;
    default:
      break;
  }
}

void draw_menu() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  OpenGL::Screen & screen = OpenGL::Screen::Instance();
  screen.setFlatProjection();
  glDisable(GL_DEPTH_TEST);

  guiManager.draw();


  glEnable(GL_DEPTH_TEST);

  SDL_GL_SwapBuffers();
}

void run_main() {
  SDL_Event event;
  Timer & t = Timer::Instance();
  t.update();
  //Uint32 now_ticks = SDL_GetTicks();
  Uint32 now_ticks = t.getRealTime();
  while(!global_Done && !global_EC) {
    while (SDL_PollEvent(&event)) {
      switch(event.type) {
        case SDL_KEYDOWN:
          handleKeyPress(&event.key.keysym);
          break;
        case SDL_MOUSEBUTTONDOWN:
          guiManager.receive(event.button);
          break;
          /*case SDL_KEYUP:
            handleKeyUp(&event.key.keysym);
            break;*/
        case SDL_VIDEORESIZE:
          OpenGL::Screen::Instance().resize(event.resize.w, event.resize.h);
          break;
        case SDL_QUIT:
          global_Done = 1;
          break;
        default:
          break;
      }
    }
    draw_menu();
    //now_ticks = SDL_GetTicks();
    t.update();
    now_ticks = t.getRealTime(); 
    guiManager.update(now_ticks);
    SDL_Delay(20);
  }
  // otherwise only at exit, which... troubles loki::smallobject
  guiManager.clearCache();
  Timer::Instance().clearAllEvents();
}

#endif
