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
#include <SDL_opengl.h>
#include "gl_spritecache.h"
#include "dataholder.h"
#include "spritemanager.h"
#include "log.h"
#include "timer.h"

namespace OpenGTA {
  SpriteManager::SpriteManager() {
    drawMode = (1);

    registerAnimation(0, SpriteObject::Animation(0,  0)); // dummy

    registerAnimation(1, SpriteObject::Animation(98, 0)); // standing still
    registerAnimation(2, SpriteObject::Animation(0,  7, 0.001f)); // walking
    registerAnimation(3, SpriteObject::Animation(8,  7, 0.0015f)); // running

//    registerAnimation(3, SpriteObject::Animation(16,  0)); // sitting in car
//    registerAnimation(4, SpriteObject::Animation(17, 7)); // car-exit
//    registerAnimation(5, SpriteObject::Animation(25, 7)); // car-enter
    //registerAnimation(3, SpriteObject::Animation(107, 7, 0.002f));
    //registerAnimation(4, SpriteObject::Animation(99, 7, 0.001f));
    //registerAnimation(5, SpriteObject::Animation(28, 7));
//    registerAnimation(6, SpriteObject::Animation(38, 2)); // falling
    registerAnimation(7, SpriteObject::Animation(41, 0)); // sliding under
    registerAnimation(8, SpriteObject::Animation(42, 1)); // death pose; maybe just 1?
    registerAnimation(9, SpriteObject::Animation(44, 0)); // death-back pose
    registerAnimation(10, SpriteObject::Animation(45, 1)); // shot-in-front
    registerAnimation(11, SpriteObject::Animation(47, 1)); // swimming 
    registerAnimation(12, SpriteObject::Animation(98, 0)); // standing still

    registerAnimation(4, SpriteObject::Animation(89, 0)); // standing, gun
    registerAnimation(5, SpriteObject::Animation(99, 7, 0.001f)); // walking, gun
    registerAnimation(6, SpriteObject::Animation(107, 7, 0.002f)); // running, gun
    /*
    registerAnimation(12, SpriteObject::Animation(
    registerAnimation(13, SpriteObject::Animation(
    registerAnimation(14, SpriteObject::Animation(
    registerAnimation(15, SpriteObject::Animation(
    registerAnimation(16, SpriteObject::Animation(
    registerAnimation(17, SpriteObject::Animation(
    registerAnimation(18, SpriteObject::Animation(
    registerAnimation(19, SpriteObject::Animation(
*/
  }
  SpriteManager::~SpriteManager() {
    clear();
    animations.clear();
  }

  void SpriteManager::update(Uint32 ticks) {
    for (PedListType::iterator i = activePeds.begin(); i != activePeds.end(); ++i) {
      for (ObjectListType::iterator j = activeObjects.begin(); j != activeObjects.end(); ++j) {
        Pedestrian & ped = *i;
        GameObject & obj = *j;
        Vector3D d(ped.GetCenterPoint() - obj.GetCenterPoint());
        if (d.SquareMagnitude() < 25) {
          INFO << "obj: " << obj.pos.x << ", " << obj.pos.y << "[" << obj.m_Extent.y << "], " << obj.pos.z << std::endl;
          INFO << "ped: " << ped.pos.x << ", " << ped.pos.y <<", " << ped.pos.z << std::endl;
          INFO << "ped in obj: " << ped.IsBoxInBox(obj) << std::endl;
          INFO << "obj in ped: " << obj.IsBoxInBox(ped) << std::endl;
        }
      }
    }
    for (PedListType::iterator i = activePeds.begin(); i != activePeds.end(); ++i) {
      i->update(ticks);
    }
  }

  void SpriteManager::drawInRect(SDL_Rect & r) {
    for (PedListType::iterator i = activePeds.begin(); i != activePeds.end(); ++i) {
      Pedestrian & ped = (*i);
      if ((ped.pos.x >= r.x) && (ped.pos.x <= r.x + r.w) &&
        (ped.pos.z >= r.y) && (ped.pos.z <= r.y + r.h)) {
          drawPed(ped);
        }
    }
    for (ObjectListType::iterator i = activeObjects.begin(); i != activeObjects.end(); ++i) {
      GameObject & obj = (*i);
      if ((obj.pos.x >= r.x) && (obj.pos.x <= r.x + r.w) &&
        (obj.pos.z >= r.y) && (obj.pos.z <= r.y + r.h)) {
          drawObject(obj);
        }
    }
    for (CarListType::iterator i = activeCars.begin(); i != activeCars.end(); ++i) {
      Car & car = *i;
      if ((car.pos.x >= r.x) && (car.pos.x <= r.x + r.w) &&
        (car.pos.z >= r.y) && (car.pos.z <= r.y + r.h)) {
          drawCar(car);
        }
    }
  }

  void SpriteManager::clear() {
    activePeds.clear();
    activeObjects.clear();
    activeCars.clear();
  }

/*
  void SpriteManager::drawCar(Car & car) {
    GraphicsBase & style = StyleHolder::Instance().get();

    GraphicsBase::SpriteInfo * info = style.getSprite(sprNum);

    OpenGL::PagedTexture t;
    GLfloat w, h;
    
    drawGL(t, w, h);

  }
  */

#define GL_OBJ_COMMON(o) GL_CHECKERROR; \
    glPushMatrix(); \
    glTranslatef(o.pos.x, o.pos.y, o.pos.z); \
    glRotatef(o.rot, 0, 1, 0); \
    glGetFloatv(GL_MODELVIEW_MATRIX, *o.m_M.m)


  void SpriteManager::drawCar(Car & car) {
    GL_OBJ_COMMON(car);
    GraphicsBase & style = StyleHolder::Instance().get();
    OpenGL::PagedTexture t;
    PHYSFS_uint16 sprNum = style.spriteNumbers.reIndex(car.sprNum + 
      car.anim.firstFrameOffset + car.anim.currentFrame, car.sprType);
    
    GraphicsBase::SpriteInfo * info = style.getSprite(sprNum);
    assert(info);
    float w = float(info->w) / 64.0f;
    float h = float(info->h) / 64.0f;
    if (OpenGL::SpriteCacheHolder::Instance().has(sprNum, car.remap))
      t = OpenGL::SpriteCacheHolder::Instance().get(sprNum, car.remap);
    else {
      t = OpenGL::SpriteCacheHolder::Instance().create(car.sprNum + 
        car.anim.firstFrameOffset + car.anim.currentFrame, 
        car.sprType, car.remap);
    }
    glBindTexture(GL_TEXTURE_2D, t.inPage);

    glBegin(GL_QUADS);
    glTexCoord2f(t.coords[0].u, t.coords[1].v);
    glVertex3f(-w/2, 0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[1].v);
    glVertex3f(w/2,  0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[0].v);
    glVertex3f(w/2,  0.0f, -h/2);
    glTexCoord2f(t.coords[0].u, t.coords[0].v);
    glVertex3f(-w/2, 0.0f, -h/2);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (getDrawBBox()) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(-car.m_Extent.x, 0.0f, car.m_Extent.z);
      glVertex3f(car.m_Extent.x,  0.0f, car.m_Extent.z);
      glVertex3f(car.m_Extent.x,  0.0f, -car.m_Extent.z);
      glVertex3f(-car.m_Extent.x, 0.0f, -car.m_Extent.z);
      glVertex3f(-car.m_Extent.x, 0.0f, car.m_Extent.z);
      glEnd();
      glBegin(GL_LINE_STRIP);
      glVertex3f(-car.m_Extent.x, car.m_Extent.y, car.m_Extent.z);
      glVertex3f(car.m_Extent.x,  car.m_Extent.y, car.m_Extent.z);
      glVertex3f(car.m_Extent.x,  car.m_Extent.y, -car.m_Extent.z);
      glVertex3f(-car.m_Extent.x, car.m_Extent.y, -car.m_Extent.z);
      glVertex3f(-car.m_Extent.x, car.m_Extent.y, car.m_Extent.z);
      glEnd();
    }
    if (getDrawTexBorder()) {
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_LINE_STRIP);
      glColor3f(float(202)/255.0f, float(31)/255.0f, float(123)/255.0f);
      glVertex3f(-w/2, 0.0f, h/2);
      glVertex3f(w/2,  0.0f, h/2);
      glVertex3f(w/2,  0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, h/2);
      glEnd();
      glColor3f(1.0f, 1.0f, 1.0f);
    }
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    GL_CHECKERROR;
  }

  void SpriteManager::drawObject(GameObject & obj) {
    GL_OBJ_COMMON(obj);
    GraphicsBase & style = StyleHolder::Instance().get();
    OpenGL::PagedTexture t;
    PHYSFS_uint16 sprNum = style.spriteNumbers.reIndex(obj.sprNum + 
      obj.anim.firstFrameOffset + obj.anim.currentFrame, obj.sprType);
    
    GraphicsBase::SpriteInfo * info = style.getSprite(sprNum);
    assert(info);
    float w = float(info->w) / 64.0f;
    float h = float(info->h) / 64.0f;
    if (OpenGL::SpriteCacheHolder::Instance().has(sprNum, obj.remap))
      t = OpenGL::SpriteCacheHolder::Instance().get(sprNum, obj.remap);
    else {
      t = OpenGL::SpriteCacheHolder::Instance().create(obj.sprNum + 
        obj.anim.firstFrameOffset + obj.anim.currentFrame, 
        obj.sprType, obj.remap);
    }
    glBindTexture(GL_TEXTURE_2D, t.inPage);

    glBegin(GL_QUADS);
    glTexCoord2f(t.coords[0].u, t.coords[1].v);
    glVertex3f(-w/2, 0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[1].v);
    glVertex3f(w/2,  0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[0].v);
    glVertex3f(w/2,  0.0f, -h/2);
    glTexCoord2f(t.coords[0].u, t.coords[0].v);
    glVertex3f(-w/2, 0.0f, -h/2);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (getDrawBBox()) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(-obj.m_Extent.x, 0.0f, obj.m_Extent.z);
      glVertex3f(obj.m_Extent.x,  0.0f, obj.m_Extent.z);
      glVertex3f(obj.m_Extent.x,  0.0f, -obj.m_Extent.z);
      glVertex3f(-obj.m_Extent.x, 0.0f, -obj.m_Extent.z);
      glVertex3f(-obj.m_Extent.x, 0.0f, obj.m_Extent.z);
      glEnd();
      glBegin(GL_LINE_STRIP);
      glVertex3f(-obj.m_Extent.x, obj.m_Extent.y, obj.m_Extent.z);
      glVertex3f(obj.m_Extent.x,  obj.m_Extent.y, obj.m_Extent.z);
      glVertex3f(obj.m_Extent.x,  obj.m_Extent.y, -obj.m_Extent.z);
      glVertex3f(-obj.m_Extent.x, obj.m_Extent.y, -obj.m_Extent.z);
      glVertex3f(-obj.m_Extent.x, obj.m_Extent.y, obj.m_Extent.z);
      glEnd();
    }
    if (getDrawTexBorder()) {
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_LINE_STRIP);
      glColor3f(float(202)/255.0f, float(31)/255.0f, float(123)/255.0f);
      glVertex3f(-w/2, 0.0f, h/2);
      glVertex3f(w/2,  0.0f, h/2);
      glVertex3f(w/2,  0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, h/2);
      glEnd();
      glColor3f(1.0f, 1.0f, 1.0f);
    }
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    GL_CHECKERROR;

  }

  void SpriteManager::drawPed(Pedestrian & ped) {
    GL_OBJ_COMMON(ped);
  /*
    GL_CHECKERROR;
    glPushMatrix();
    glTranslatef(ped.pos.x, ped.pos.y, ped.pos.z);
    glRotatef(ped.rot, 0, 1, 0);
    glGetFloatv(GL_MODELVIEW_MATRIX, *ped.m_M.m);



    for (int i=0; i < 4; i++) {
      for (int j=0; j <4 ;j++) {
        std::cout << ped.m_M.m[i][j] << " ";
      }
      std::cout << std::endl;
    }
    std::cout << std::endl;
    */

    GraphicsBase & style = StyleHolder::Instance().get();
    
    
    OpenGL::PagedTexture t;
    PHYSFS_uint16 sprNum = style.spriteNumbers.reIndex(ped.sprNum + 
      ped.anim.firstFrameOffset + ped.anim.currentFrame, ped.sprType);
    
    GraphicsBase::SpriteInfo * info = style.getSprite(sprNum);
    assert(info);
    float w = float(info->w) / 64.0f;
    float h = float(info->h) / 64.0f;


    if (OpenGL::SpriteCacheHolder::Instance().has(sprNum, ped.remap))
      t = OpenGL::SpriteCacheHolder::Instance().get(sprNum, ped.remap);
    else {
      t = OpenGL::SpriteCacheHolder::Instance().create(ped.sprNum + 
        ped.anim.firstFrameOffset + ped.anim.currentFrame, 
        ped.sprType, ped.remap);
    }
    glBindTexture(GL_TEXTURE_2D, t.inPage);

    glBegin(GL_QUADS);
    glTexCoord2f(t.coords[0].u, t.coords[1].v);
    glVertex3f(-w/2, 0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[1].v);
    glVertex3f(w/2,  0.0f, h/2);
    glTexCoord2f(t.coords[1].u, t.coords[0].v);
    glVertex3f(w/2,  0.0f, -h/2);
    glTexCoord2f(t.coords[0].u, t.coords[0].v);
    glVertex3f(-w/2, 0.0f, -h/2);

    glEnd();
    glDisable(GL_TEXTURE_2D);
    if (getDrawBBox()) {
      glBegin(GL_LINE_STRIP);
      glVertex3f(-ped.m_Extent.x, 0.0f, ped.m_Extent.z);
      glVertex3f(ped.m_Extent.x,  0.0f, ped.m_Extent.z);
      glVertex3f(ped.m_Extent.x,  0.0f, -ped.m_Extent.z);
      glVertex3f(-ped.m_Extent.x, 0.0f, -ped.m_Extent.z);
      glVertex3f(-ped.m_Extent.x, 0.0f, ped.m_Extent.z);
      glEnd();
    }
    if (getDrawTexBorder()) {
      glDisable(GL_TEXTURE_2D);
      glBegin(GL_LINE_STRIP);
      glColor3f(float(202)/255.0f, float(31)/255.0f, float(123)/255.0f);
      glVertex3f(-w/2, 0.0f, h/2);
      glVertex3f(w/2,  0.0f, h/2);
      glVertex3f(w/2,  0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, -h/2);
      glVertex3f(-w/2, 0.0f, h/2);
      glEnd();
      glColor3f(1.0f, 1.0f, 1.0f);
    }
    glEnable(GL_TEXTURE_2D);

    glPopMatrix();
    GL_CHECKERROR;
  }

  void SpriteManager::addPed(Pedestrian & ped) {
    activePeds.push_back(ped);
  }

  Pedestrian & SpriteManager::getPedById(const Uint32 & id) {
    PedListType::iterator i = activePeds.begin();
    while (i != activePeds.end()) {
      if (i->pedId == id)
        return *i;
      ++i;
    }
    assert(0);
    return *activePeds.begin();
  }

  void SpriteManager::removePedById(const Uint32 & id) {
    PedListType::iterator i = activePeds.begin();
    while (i != activePeds.end()) {
      if (i->pedId == id) {
        activePeds.erase(i);
        return;
      }
      ++i;
    }
    WARN << "didn't find ped id " << id << " -- cannot remove"<<std::endl;

  }

  void SpriteManager::addCar(Car & car) {
    activeCars.push_back(car);
  }

  Car & SpriteManager::getCarById(const Uint32 & id) {
    CarListType::iterator i = activeCars.begin();
    while (i != activeCars.end()) {
      if (i->carId == id) {
        return *i;
      }
      ++i;
    }
    assert(0);
    return *activeCars.begin();
  }
  
  void SpriteManager::addObject(GameObject & go) {
    activeObjects.push_back(go);
  }

  GameObject & SpriteManager::getObjectById(const Uint32 & id) {
    ObjectListType::iterator i = activeObjects.begin();
    while (i != activeObjects.end()) {
      if (i->objId == id) {
        return *i;
      }
      ++i;
    }
    assert(0);
    return *activeObjects.begin();
  }

  SpriteObject::Animation & SpriteManager::getAnimationById(const Uint32 & id) {
    AnimLookupType::iterator i = animations.find(id);
    assert(i != animations.end());
    return i->second;
  }

  void SpriteManager::registerAnimation(const Uint32 & id, 
    const SpriteObject::Animation & anim) {
    animations.insert(std::make_pair<Uint32, SpriteObject::Animation>(id, anim));
  }

  void SpriteManager::setDrawBBox(bool v) {
    if (v)
      drawMode = (drawMode | 4);
    else
      drawMode = (drawMode & 4 ? drawMode - 4 : drawMode);
  }
  
  void SpriteManager::setDrawTexBorder(bool v) {
    if (v)
      drawMode = (drawMode | 2);
    else
      drawMode = (drawMode & 2 ? drawMode - 2 : drawMode);  
  }

  void SpriteManager::setDrawTexture(bool v) {
  }

  void SpriteManager::createProjectile(uint8_t typeId, float r, Vector3D p, Vector3D d, Uint32 & ticks) {
    activeProjectiles.push_back(Projectile(typeId, r, p, d, ticks));
  }
}
