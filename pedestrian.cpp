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
#include "pedestrian.h"
#include "spritemanager.h"
#include "opengta.h"
#include "dataholder.h"
#include "log.h"

float slope_height_offset(unsigned char slope_type, float dx, float dz);
namespace OpenGTA {
  Projectile::Projectile(uint8_t id, float r, Vector3D p, Vector3D d, Uint32 now) :
    typeId(id), rot(r), pos(p), delta(d), endsAtTick(now + 5000) {}

  SpriteObject::Animation::Animation() :
    Util::Animation(7, 7),
    firstFrameOffset(0), moveSpeed(0.0f) {}

  SpriteObject::Animation::Animation(const Animation & other) :
    Util::Animation(other.numFrames, 1000 / other.delay),
    firstFrameOffset(other.firstFrameOffset), 
    //numFrames(other.numFrames),
    moveSpeed(other.moveSpeed) {}
  
  SpriteObject::Animation::Animation(Uint16 foff, Uint8 num) :
    Util::Animation(num, 7),
    firstFrameOffset(foff), moveSpeed(0.0f) {}

  SpriteObject::Animation::Animation(Uint16 foff, Uint8 num, float speed) :
    Util::Animation(num, 7),
    firstFrameOffset(foff), moveSpeed(speed) {}
  
  SpriteObject::SpriteObject(const Vector3D & p) :
    pos(p), //animRef(&SpriteManagerHolder::Instance().getAnimationById(0)) {
    anim(SpriteManagerHolder::Instance().getAnimationById(0)) {
    sprNum = 0;
    remap = -1;
    //curFrame = 0;
    lastFrameUpdateAt = 0;
    lastUpdateAt = 0;
    rot = 0.0f;
    animActive = false;
    sprType = GraphicsBase::SpriteNumbers::ARROW;
  }
  
  Uint8 SpriteObject::calcCurrentFrame(Uint32 ticks) {
    Uint32 delta = ticks - lastFrameUpdateAt;
    //assert(animRef);
    if (delta > 100) {
      //curFrame += 1;
      //INFO << "new frame: " << int(curFrame) << " total: " << sprNum + curFrame + anim.firstFrameOffset << std::endl;
      lastFrameUpdateAt = ticks;
    }
    /*
    if (curFrame > anim.numFrames)
      curFrame = 0;
    */
    return 0;//curFrame;
  }

  SpriteObject::SpriteObject(const SpriteObject & other) :
    pos(other.pos), anim(other.anim) {
    copyValues(other);
  }

  void SpriteObject::copyValues(const SpriteObject & other) {
    sprNum = other.sprNum;
    //curFrame = other.curFrame;
    remap = other.remap;
    lastFrameUpdateAt = other.lastFrameUpdateAt;
    lastUpdateAt = other.lastUpdateAt;
    rot = other.rot;
    sprType = other.sprType;
    animActive = other.animActive;
  }

  float SpriteObject::heightOverTerrain(const Vector3D & v) {
    float x, y, z;
    x = floor(v.x);
    y = floor(v.y);
    z = floor(v.z);
    PHYSFS_uint8 x_b, z_b;
    x_b = (PHYSFS_uint8)x;
    z_b = (PHYSFS_uint8)z;
    if (y < 0.0f) {
      ERROR << "Below level! at coords: " << v.x << ", " << v.y << ", " << v.z << std::endl;
      return 1.0f;
    }
    OpenGTA::Map & map = OpenGTA::MapHolder::Instance().get();
    while (y >= map.getNumBlocksAtNew(x_b, z_b) && y > 0.0f)
      y -= 1.0f;
    while (y < map.getNumBlocksAtNew(x_b, z_b) && y > 0.0f) {
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(x_b, z_b, (PHYSFS_uint8)y);
      assert(block);
      if (block->blockType() > 0) {
        float bz = slope_height_offset(block->slopeType(), v.x - x, v.z - z);
        if (block->slopeType() == 0 && block->blockType() != 5)
          bz -= 1.0f;
        return v.y - (y + bz);
      }
      y -= 1.0f;
    }
    y = floor(v.y) + 1.0f;
    while (y < map.getNumBlocksAtNew(x_b, z_b) && y > 0.0f) {
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(x_b, z_b, (PHYSFS_uint8)y);
      assert(block);
      if (block->blockType() > 0) {
        float bz = slope_height_offset(block->slopeType(), v.x - x, v.z - z);
        if (block->slopeType() == 0 && block->blockType() != 5)
          bz -= 1.0f;
        return v.y - (y + bz);
      }
      y += 1.0f;
    }
    INFO << "should this be reached?" << std::endl;
    return 1.0f;
  }

  Pedestrian::Pedestrian(const Vector3D & e,
    const Vector3D & p) : SpriteObject(p), 
      OBox(RollMatrix3D(0), e * 0.5f) {
    m_M.Translate(p);
    pedId = 0;
    m_control = 0;
    animId = 0;
    inGroundContact = 0;
    sprType = GraphicsBase::SpriteNumbers::PED;
    activeWeapon = 0;
    speedForces = Vector3D(0, 0, 0);
    weaponReloadedAt = 0;
  }

  Pedestrian::Pedestrian(const Vector3D & e,
    const Vector3D & p, const Uint32 & asId) : SpriteObject(p),
      OBox(RollMatrix3D(0), e * 0.5f) {
    m_M.Translate(p);
    pedId = asId;
    m_control = 0;
    animId = 0;
    inGroundContact = 0;
    sprType = GraphicsBase::SpriteNumbers::PED;
    activeWeapon = 0;
    speedForces = Vector3D(0, 0, 0);
    weaponReloadedAt = 0;
  }

  Pedestrian::Pedestrian(const Pedestrian & other) : SpriteObject(other),
    OBox(other.m_M, other.m_Extent) {
    //animRef(SpriteManagerHolder::Instance().getAnimationById(other.animId)) { 
    copyValues(other);
  }

  void Pedestrian::switchToAnim(const Uint32 & newId) {
    anim = Animation(SpriteManagerHolder::Instance().getAnimationById(newId));
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::LOOP);
    animId = newId;
    //curFrame = 0;
  }

  void SpriteObject::setAnimation(Animation & otherAnim) {
    anim = Animation(otherAnim);
    //curFrame = 0;
    // FIXME: animId?
  }

  void Pedestrian::copyValues(const Pedestrian & other) {

    m_control = other.m_control;
    animId = other.animId;
    pedId = other.pedId;
    inGroundContact = other.inGroundContact;
    sprType = other.sprType;
    speedForces = other.speedForces;
    activeWeapon = other.activeWeapon;
    inventory = other.inventory;
    weaponReloadedAt = other.weaponReloadedAt;
  }

  void Pedestrian::giveItem(uint8_t id, uint32_t amount) {
    InventoryType::iterator i = inventory.find(id);
    if (i == inventory.end())
      inventory[id] = amount;
    else
      i->second += amount;
  }

  void Pedestrian::tryMove(Vector3D nPos) {
    float x, y, z;
    x = floor(nPos.x);
    y = floor(nPos.y);
    z = floor(nPos.z);
    OpenGTA::Map & map = OpenGTA::MapHolder::Instance().get();
    OpenGTA::GraphicsBase & graphics = OpenGTA::StyleHolder::Instance().get();
    //INFO << heightOverTerrain(nPos) << std::endl;
    float hot = heightOverTerrain(nPos);
    if (hot > 0.3f)
      inGroundContact = 0;
    else if (hot < 0.0) {
      INFO << "gone below: " << hot << " at " << nPos.x << ", " << nPos.y << ", " << nPos.z << std::endl;
      nPos.y -= (hot - 0.3f);
      INFO << nPos.y << std::endl;
      inGroundContact = 1;
    }
    else {
      inGroundContact = 1;
    }
    if (y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z)) && y > 0.0f) {
      //INFO << x << ", " << y << ", " << z << ": " << int(map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z))) << std::endl;
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z), PHYSFS_uint8(y));
      assert(block);/*
      if (block->blockType() > 0) {
        float bz = slope_height_offset(block->slopeType(), nPos.x - x, nPos.z - z);
        if (block->slopeType() == 0)
          bz -= 1.0f;
        if (inGroundContact) {
          nPos.y = y + bz + 0.3f;
          //pos = nPos;
        }
        else if (nPos.y - y - bz < 0.3f) {
      INFO << "ped near ground (type: " << int(block->blockType()) << " float-pos: " 
        << nPos.x << ", " << nPos.y << ", " << nPos.z << std::endl;
          inGroundContact = 1;
          nPos.y = y + bz + 0.3f;
          INFO << "height rewritten to: " << nPos.y << std::endl;
          //pos = nPos;
          
        }
      }
      else {
        inGroundContact = 0;
        INFO << "lost footing: " << x << ", " << y << ", " << z << std::endl;
      }*/
      if (block->left && graphics.isBlockingSide(block->left)) { // && block->isFlat() == false) {
        if (block->isFlat()) {
        //INFO << "xblock left: " << x - pos.x << std::endl;
          if (x - pos.x < 0 && x - pos.x > -0.2f) {
            nPos.x = (nPos.x < pos.x) ? pos.x : nPos.x;
          }
          else if (x - pos.x > 0 && x - pos.x < 0.2f)
            nPos.x = pos.x;
        }
        else {
        INFO << "xblock left: " << x - pos.x << " tex: " << int(block->left) << std::endl;
          if (x - pos.x > 0 && x - pos.x < 0.2f)
            nPos.x = pos.x;
          else if (x - pos.x < 0 && x - pos.x > -0.2f)
            nPos.x = (nPos.x < pos.x) ? pos.x : nPos.x;
        }
      }
      if (block->right && block->isFlat() == false) {
        INFO << "xblock right: " << pos.x - x - 1 << " tex: " << int(block->right) << std::endl;
        if (pos.x - x - 1 > 0 && pos.x - x - 1 < 0.2f) {
          nPos.x = pos.x;
        }
        else if (pos.x - x - 1 < 0 && pos.x - x - 1 > -0.2f)
          nPos.x = (nPos.x > pos.x) ? pos.x : nPos.x;
      }
      if (block->top && graphics.isBlockingSide(block->top)) { // && block->isFlat() == false) {
        if (block->isFlat()) {
        INFO << "zblock top: " << z - pos.z << " tex: " << int(block->top) << std::endl;
          if (z - pos.z > 0 && z - pos.z < 0.2f)
            nPos.z = pos.z;
          else if (z - pos.z < 0 && z - pos.z > -0.2f)
            nPos.z = (nPos.z < pos.z) ? pos.z : nPos.z;
        }
        else {
        INFO << "zblock top: " << z - pos.z << " tex: " << int(block->top)<<  std::endl;
          if (z - pos.z > 0 && z - pos.z < 0.2f)
            nPos.z = pos.z;
          else if (z - pos.z < 0 && z - pos.z > -0.2f)
            nPos.z = (nPos.z < pos.z) ? pos.z : nPos.z;
        }
      }
      if (block->bottom && block->isFlat() == false) {
        INFO << "zblock bottom: " << pos.z - z - 1<< " tex: " << int(block->bottom)<< std::endl;
        if (pos.z - z - 1 > 0 && pos.z - z - 1 < 0.2f) {
          nPos.z = pos.z;
        }
        else if (pos.z - z - 1 < 0 && pos.z - z - 1 > -0.2f)
          nPos.z = (nPos.z > pos.z) ? pos.z : nPos.z;
      }
      if (x >= 1 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x-1), PHYSFS_uint8(z))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x-1), PHYSFS_uint8(z), PHYSFS_uint8(y));
        if (block->right && block->isFlat() == false) {
          INFO << "xblock right: " << pos.x - x << " tex: " << int(block->right)<< std::endl;
          if (pos.x - x < 0.2f) {
            nPos.x = (nPos.x < pos.x ? pos.x : nPos.x);
          }
        }
      }
      if (x < 255 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x+1), PHYSFS_uint8(z))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x+1), PHYSFS_uint8(z), PHYSFS_uint8(y));
        if (block->left && graphics.isBlockingSide(block->left)) { // && block->isFlat() == false) {
          INFO << "xblock left: " << x + 1 - pos.x << " tex: " << int(block->left)<< std::endl;
          if (block->isFlat()) {
            if (x + 1 - pos.x > 0 && x + 1 - pos.x < 0.2f)
              nPos.x = (nPos.x < pos.x ? nPos.x : pos.x);
          }
          else {
            if (x + 1 - pos.x < 0.2f)
              nPos.x = (nPos.x < pos.x ? nPos.x : pos.x);
          }
        }
      }
      if (z >= 1 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z-1))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z-1), PHYSFS_uint8(y));
        if (block->bottom && block->isFlat() == false) {
          INFO << "zblock bottom: " << pos.z - z<< " tex: " << int(block->bottom)<< std::endl;
          if (pos.z - z < 0.2f) {
            nPos.z = (nPos.z < pos.z ? pos.z : nPos.z);
          }
        }
      }
      if (z < 255 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z+1))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z+1), PHYSFS_uint8(y));
        if (block->top && graphics.isBlockingSide(block->top)) { // && block->isFlat() == false) {
          INFO << "zblock top: " << z + 1 - pos.z<< " tex: " << int(block->top) << std::endl;
          if (block->isFlat()) {
            if (z + 1 - pos.z > 0 && z + 1 - pos.z < 0.2f)
              nPos.z = (nPos.z < pos.z ? nPos.z : pos.z);
          }
          else {
            if (z + 1 - pos.z < 0.2f)
              nPos.z = (nPos.z < pos.z ? nPos.z : pos.z);
          }
        }
      }
      //if (inGroundContact)
      //  pos = nPos;
    }
    if (inGroundContact)
      pos = nPos;
    //else
    //  inGroundContact = 0;

  }

  void Pedestrian::equip(uint8_t id) {
    if (id == 0) {
      activeWeapon = 0;
    }
    else {
      InventoryType::iterator i = inventory.find(id);
      if (i != inventory.end()) {
        activeWeapon = i->first;
      }
      else
        ERROR << "Ped does not have item type " << int(id) << std::endl;
    }
  }

  void Pedestrian::fireWeapon(Uint32 ticks) {
    if (activeWeapon == 0)
      return; // FIXME: punching!
    InventoryType::iterator i = inventory.find(activeWeapon);
    if (i->second == 0)
      return; // no ammo
    if (ticks < weaponReloadedAt)
      return;

    weaponReloadedAt = ticks + 2000;
    OpenGTA::SpriteManagerHolder::Instance().createProjectile(i->first, rot, pos, Vector3D(0.2f, 0, 0.2f), ticks);

  }

  void Pedestrian::update(Uint32 ticks) {
    // update the animation
    if (m_control) {
      switch(m_control->move) {
        case 1:
          if (!(animId == 2u + activeWeapon*3))
            switchToAnim(2 + activeWeapon*3);
          break;
        case 2:
          if (!(animId == 3u + activeWeapon*3))
            switchToAnim(3 + activeWeapon*3);
          break;
        case 0:
          if (!(animId == 1u + activeWeapon*3))
            switchToAnim(1 + activeWeapon*3);
          break;
        case -1:
          if (!(animId == 2u + activeWeapon*3)) {
            switchToAnim(2 + activeWeapon*3);
            anim.set(Util::Animation::PLAY_BACKWARD, Util::Animation::LOOP);
          }
      }
    }
    anim.update(ticks);
    // update position/rotation
    Uint32 delta = ticks - lastUpdateAt;
    Vector3D moveDelta(0, 0, 0);
    if (m_control) {
      switch(m_control->turn) {
        case -1:
          rot -= 0.2f * delta;
          break;
        case 1:
          rot += 0.2f * delta;
          break;
        case 0:
          break;
      }
      if (rot >= 360.0f)
        rot -= 360.0f;
      if (rot < 0.0f)
        rot += 360.0f;
      switch(m_control->move) {
        case -1:
          moveDelta.x -= sin(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          moveDelta.z -= cos(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          break;
        case 1:
          moveDelta.x += sin(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          moveDelta.z += cos(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          break;
        case 2:
          moveDelta.x += sin(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          moveDelta.z += cos(rot * M_PI / 180.0f) * anim.moveSpeed * delta;
          break;
        case 0:
          break;
      }
    }
    tryMove(pos + moveDelta);
    if (!inGroundContact) {
      speedForces.y += 0.1f;
      pos.y -= speedForces.y;
      if (speedForces.y < 0.2f)
        INFO << "bridge step? height: " << pos.y << " speed: " << speedForces.y << std::endl;
      else
        INFO << "FALLING" << pos.y << " speed " << speedForces.y << std::endl;
    }
    else {
      if (speedForces.y > 0.1)
        INFO << "impacting with speed: " << speedForces.y << std::endl;
      speedForces.y = 0.0f;
    }
    lastUpdateAt = ticks;

  }

#define INT2FLOAT_WRLD(c) (float(c >> 6) + float(c % 64) / 64.0f)
#define INT2F_DIV64(v) (float(v) / 64.0f)
#define INT2F_DIV128(v) (float(v) / 128.0f)

  Car::Car(const OpenGTA::Map::ObjectPosition & op) :
    SpriteObject(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    OBox(RollMatrix3D(0), Vector3D()),
    c_info(*StyleHolder::Instance().get().findCarByModel(op.type)) {
    type = op.type;
    sprType = GraphicsBase::SpriteNumbers::CAR;
    sprNum = c_info.sprNum;
    m_Extent = Vector3D(INT2F_DIV128(c_info.width),
      INT2F_DIV128(c_info.depth),
      INT2F_DIV128(c_info.height));
    m_M.Translate(pos);
    rot = op.rotation * 360 / 1024;
  }

  Car::Car(const Car & other) : SpriteObject(other),
    OBox(other.m_M, other.m_Extent),
    c_info(*StyleHolder::Instance().get().findCarByModel(other.type)) {
    copyValues(other);
  }

  void Car::copyValues(const Car & other) {
    sprType = other.sprType;
    delta = other.delta;
    carId = other.carId;
    type = other.type;
  }

  GameObject::GameObject(const OpenGTA::Map::ObjectPosition & op) :
    SpriteObject(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    OBox(RollMatrix3D(0), Vector3D()) {
    sprType = GraphicsBase::SpriteNumbers::OBJECT;
    GraphicsBase & style = StyleHolder::Instance().get();
    sprNum = style.objectInfos[op.type]->sprNum;
    m_Extent = Vector3D(INT2F_DIV128(style.objectInfos[op.type]->width),
      INT2F_DIV128(style.objectInfos[op.type]->depth),
      INT2F_DIV128(style.objectInfos[op.type]->height));
    m_M.Translate(pos);
    rot = op.rotation * 360 / 1024;

  }

  GameObject::GameObject(const GameObject & other) : SpriteObject(other),
    OBox(other.m_M, other.m_Extent) {
    copyValues(other);
  }

  void GameObject::copyValues(const GameObject & other) {
    sprType = other.sprType;
  }
}
