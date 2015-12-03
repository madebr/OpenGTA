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
#include "game_objects.h"
#include "spritemanager.h"
#include "dataholder.h"
#include "cell_iterator.h"
#include "timer.h"
#include "Functor.h"

#define INT2FLOAT_WRLD(c) (float(c >> 6) + float(c % 64) / 64.0f)
#define INT2F_DIV64(v) (float(v) / 64.0f)
#define INT2F_DIV128(v) (float(v) / 128.0f)

float slope_height_offset(unsigned char slope_type, float dx, float dz);

namespace OpenGTA {
  float GameObject_common::heightOverTerrain(const Vector3D & v) {
    float x, y, z;
    x = floor(v.x);
    y = floor(v.y);
    z = floor(v.z);
    PHYSFS_uint8 x_b, z_b;
    x_b = (PHYSFS_uint8)x;
    z_b = (PHYSFS_uint8)z;
    if (y < 0.0f) {
      //ERROR << "Below level! at coords: " << v.x << ", " << v.y << ", " << v.z << std::endl;
      return 1.0f;
    }
    if (x < 0 || x > 255 || z < 0 || z > 255) {
      //ERROR << "x = " << x << "(" << v.x << ") z = " << z << " (" << v.z << ")" << std::endl;
      throw E_OUTOFRANGE("invalid x/z pos");
    }
    if (y > 20) {
      INFO << y << " seems a bit high; going to 20" << std::endl;
      INFO << x << " " << z << std::endl;
      y = 20;
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

  Sprite::Animation::Animation() :
    Util::Animation(7, 7),
    firstFrameOffset(0), moveSpeed(0.0f) {}

  Sprite::Animation::Animation(const Animation & other) :
    //Util::Animation(other.numFrames, 1000 / other.delay),
    Util::Animation(other),
    firstFrameOffset(other.firstFrameOffset), 
    //numFrames(other.numFrames),
    moveSpeed(other.moveSpeed) {
      set(other.get(), other.getDone());
    }

  Sprite::Animation::Animation(Uint16 foff, Uint8 num) :
    Util::Animation(num, 7),
    firstFrameOffset(foff), moveSpeed(0.0f) {}

  Sprite::Animation::Animation(Uint16 foff, Uint8 num, float speed) :
    Util::Animation(num, 7),
    firstFrameOffset(foff), moveSpeed(speed) {}

  Sprite::Sprite() :
    sprNum(0), remap(-1), 
    //anim(SpriteManagerHolder::Instance().getAnimationById(0)),
    anim(), animId(),
    sprType(GraphicsBase::SpriteNumbers::ARROW) {
    }

  Sprite::Sprite(Uint16 sprN, Sint16 rem, GraphicsBase::SpriteNumbers::SpriteTypes sprT) :
    sprNum(sprN), remap(rem),
    anim(), animId(),
    sprType(sprT) {}

  Sprite::Sprite(const Sprite & other) :
    sprNum(other.sprNum), remap(other.remap), anim(other.anim), animId(other.animId),
    sprType(other.sprType) {
    }

  void Sprite::switchToAnim(const Uint32 & newId) {
    INFO << "switching to anim " << newId << std::endl;
    anim = Animation(SpriteManagerHolder::Instance().getAnimationById(newId));
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::LOOP);
    animId = newId;
  }

  Pedestrian::Pedestrian(Vector3D e, const Vector3D & p, uint32_t id, Sint16 remapId) :
    GameObject_common(p), 
    Sprite(0, remapId, GraphicsBase::SpriteNumbers::PED), 
    OBox(TranslateMatrix3D(p), e * 0.5f),
    m_control(),
    speedForces(0, 0, 0) {
      m_M = TranslateMatrix3D(p);
      m_M.RotZ(rot);
      pedId = id;
      animId = 0;
      isDead = 0;
      lastUpdateAt = TimerHolder::Instance().getRealTime();
      inGroundContact = 0;
    }

  Pedestrian::Pedestrian(const Pedestrian & other) :
    GameObject_common(other), Sprite(other), OBox(other),

    pedId(other.pedId),
    m_control(),
    speedForces(other.speedForces) {
      lastUpdateAt = other.lastUpdateAt; 
      inGroundContact = other.inGroundContact;
      animId = other.animId;
      isDead = other.isDead;
      m_M = TranslateMatrix3D(other.pos);
      m_M.RotZ(other.rot);
    }

  extern void ai_step_fake(Pedestrian*);
  void Pedestrian::update(Uint32 ticks) {
    if (isDead) {
      anim.update(ticks);
      lastUpdateAt = ticks;
      return;
    }
    if (pedId < 0xffffffff)
      ai_step_fake(this);
    uint8_t activeWeapon = m_control.getActiveWeapon(); 
    switch(m_control.getMove()) {
      case 1:
        //if (!(animId == 2u + activeWeapon*3))
        //  switchToAnim(2 + activeWeapon*3);
        if (m_control.getRunning()) {
          if (!(animId == 3u + activeWeapon*3))
            switchToAnim(3 + activeWeapon*3);
        }
        else {
          if (!(animId == 2u + activeWeapon*2))
            switchToAnim(2 + activeWeapon*2);
        }
        break;
        /*
      case 2:
        if (!(animId == 3u + activeWeapon*3))
          switchToAnim(3 + activeWeapon*3);
        break;
        */
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
    anim.update(ticks);
    Uint32 delta = ticks - lastUpdateAt;
    //INFO << "delta = " << delta  << " t: " << ticks << " lt: " << lastUpdateAt << std::endl;
    Vector3D moveDelta(0, 0, 0);
    switch(m_control.getTurn()) {
      case -1:
        rot -= 0.2f * delta;
        //INFO << "rot: "<< rot << std::endl;
        break;
      case 1:
        rot += 0.2f * delta;
        //INFO << "rot: "<< rot << std::endl;
        break;
      case 0:
        break;
    }
    if (rot >= 360.0f)
      rot -= 360.0f;
    if (rot < 0.0f)
      rot += 360.0f;
    switch(m_control.getMove()) {
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
    tryMove(pos + moveDelta);
    if (!inGroundContact) {
      speedForces.y += 0.0005f *delta;
      pos.y -= speedForces.y;
      if (speedForces.y < 0.2f)
        INFO << "bridge step? height: " << pos.y << " speed: " << speedForces.y << std::endl;
      else
        INFO << "FALLING " << pos.y << " speed " << speedForces.y << std::endl;
    }
    else {
      if (speedForces.y > 0.1)
        INFO << "impacting with speed: " << speedForces.y << std::endl;
      speedForces.y = 0.0f;
    }
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);
    if (m_control.getFireWeapon() && ticks - lastWeaponTick > 400) {
      Vector3D d1(
        //Vector3D(-cos(rot * M_PI/180.0f), 0, sin(rot * M_PI/180.0f)).Normalized() * 0.05f 
        Vector3D(sin(rot * M_PI/180.0f), 0, cos(rot * M_PI/180.0f)).Normalized() * 0.01f 
      ); 
      SpriteManagerHolder::Instance().createProjectile(0, rot, pos, d1, ticks, pedId);
      lastWeaponTick = ticks;
    }
    //INFO << pos.x << " " << pos.y << " " << pos.z << std::endl;
    lastUpdateAt = ticks;
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
    if (hot > 0.1f)
      inGroundContact = 0;
    else if (hot < 0.0) {
      INFO << "gone below: " << hot << " at " << nPos.x << ", " << nPos.y << ", " << nPos.z << std::endl;
      nPos.y -= (hot - 0.3f);
      INFO << nPos.y << std::endl;
      inGroundContact = 1;
    }
    else {
      inGroundContact = 1;
      if (isDead)
        nPos.y -= hot - 0.05f;
      else
        nPos.y -= hot - 0.1f;
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
#ifdef DEBUG_OLD_PED_BLOCK
        INFO << "xblock left: " << x - pos.x << " tex: " << int(block->left) << std::endl;
#endif
          if (x - pos.x > 0 && x - pos.x < 0.2f)
            nPos.x = pos.x;
          else if (x - pos.x < 0 && x - pos.x > -0.2f)
            nPos.x = (nPos.x < pos.x) ? pos.x : nPos.x;
        }
      }
      if (block->right && block->isFlat() == false) {
#ifdef DEBUG_OLD_PED_BLOCK
        INFO << "xblock right: " << pos.x - x - 1 << " tex: " << int(block->right) << std::endl;
#endif
        if (pos.x - x - 1 > 0 && pos.x - x - 1 < 0.2f) {
          nPos.x = pos.x;
        }
        else if (pos.x - x - 1 < 0 && pos.x - x - 1 > -0.2f)
          nPos.x = (nPos.x > pos.x) ? pos.x : nPos.x;
      }
      if (block->top && graphics.isBlockingSide(block->top)) { // && block->isFlat() == false) {
        if (block->isFlat()) {
#ifdef DEBUG_OLD_PED_BLOCK
        INFO << "zblock top: " << z - pos.z << " tex: " << int(block->top) << std::endl;
#endif
          if (z - pos.z > 0 && z - pos.z < 0.2f)
            nPos.z = pos.z;
          else if (z - pos.z < 0 && z - pos.z > -0.2f)
            nPos.z = (nPos.z < pos.z) ? pos.z : nPos.z;
        }
        else {
#ifdef DEBUG_OLD_PED_BLOCK
        INFO << "zblock top: " << z - pos.z << " tex: " << int(block->top)<<  std::endl;
#endif
          if (z - pos.z > 0 && z - pos.z < 0.2f)
            nPos.z = pos.z;
          else if (z - pos.z < 0 && z - pos.z > -0.2f)
            nPos.z = (nPos.z < pos.z) ? pos.z : nPos.z;
        }
      }
      if (block->bottom && block->isFlat() == false) {
#ifdef DEBUG_OLD_PED_BLOCK
        INFO << "zblock bottom: " << pos.z - z - 1<< " tex: " << int(block->bottom)<< std::endl;
#endif
        if (pos.z - z - 1 > 0 && pos.z - z - 1 < 0.2f) {
          nPos.z = pos.z;
        }
        else if (pos.z - z - 1 < 0 && pos.z - z - 1 > -0.2f)
          nPos.z = (nPos.z > pos.z) ? pos.z : nPos.z;
      }
      if (x >= 1 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x-1), PHYSFS_uint8(z))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x-1), PHYSFS_uint8(z), PHYSFS_uint8(y));
        if (block->right && block->isFlat() == false) {
#ifdef DEBUG_OLD_PED_BLOCK
          INFO << "xblock right: " << pos.x - x << " tex: " << int(block->right)<< std::endl;
#endif
          if (pos.x - x < 0.2f) {
            nPos.x = (nPos.x < pos.x ? pos.x : nPos.x);
          }
        }
      }
      if (x < 255 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x+1), PHYSFS_uint8(z))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x+1), PHYSFS_uint8(z), PHYSFS_uint8(y));
        if (block->left && graphics.isBlockingSide(block->left)) { // && block->isFlat() == false) {
#ifdef DEBUG_OLD_PED_BLOCK
          INFO << "xblock left: " << x + 1 - pos.x << " tex: " << int(block->left)<< std::endl;
#endif
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
#ifdef DEBUG_OLD_PED_BLOCK
          INFO << "zblock bottom: " << pos.z - z<< " tex: " << int(block->bottom)<< std::endl;
#endif
          if (pos.z - z < 0.2f) {
            nPos.z = (nPos.z < pos.z ? pos.z : nPos.z);
          }
        }
      }
      if (z < 255 && y < map.getNumBlocksAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z+1))) {
        block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z+1), PHYSFS_uint8(y));
        if (block->top && graphics.isBlockingSide(block->top)) { // && block->isFlat() == false) {
#ifdef DEBUG_OLD_PED_BLOCK
          INFO << "zblock top: " << z + 1 - pos.z<< " tex: " << int(block->top) << std::endl;
#endif
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

  void Pedestrian::die() {
    INFO << "DIE!!!" << std::endl; 
    switchToAnim(42);
  }

  void Pedestrian::getShot(bool front) {
    isDead = 1;
    switchToAnim(45);
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::FCALLBACK);
    Loki::Functor<void> cmd(this, &Pedestrian::die);
    anim.setCallback(cmd);
  }

  Car::Car(OpenGTA::Map::ObjectPosition& op, uint32_t id) :
    GameObject_common(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    Sprite(0, -1, GraphicsBase::SpriteNumbers::CAR), OBox(),
    carInfo(*StyleHolder::Instance().get().findCarByModel(op.type)){
    carId = id;
    type = op.type;
    sprNum = carInfo.sprNum;
    m_Extent = Vector3D(INT2F_DIV128(carInfo.width),
      INT2F_DIV128(carInfo.depth),
      INT2F_DIV128(carInfo.height));
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);

    rot = op.rotation * 360 / 1024;
  }

  Car::Car(const Car & other) :
    GameObject_common(other), Sprite(other), OBox(other),
    carInfo(*StyleHolder::Instance().get().findCarByModel(other.type)) {
    type = other.type;
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);

    carId = other.carId;
  }

  void Car::update(Uint32 ticks) {
  }

  SpriteObject::SpriteObject(OpenGTA::Map::ObjectPosition& op, uint32_t id) :
    GameObject_common(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    Sprite(0, -1, GraphicsBase::SpriteNumbers::OBJECT), OBox() {
    objId = id;
    GraphicsBase & style = StyleHolder::Instance().get();
    sprNum = style.objectInfos[op.type]->sprNum;
    m_Extent = Vector3D(INT2F_DIV128(style.objectInfos[op.type]->width),
      INT2F_DIV128(style.objectInfos[op.type]->depth),
      INT2F_DIV128(style.objectInfos[op.type]->height));
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);
    rot = op.rotation * 360 / 1024;
    isActive = true;
  }

  SpriteObject::SpriteObject(Vector3D pos, Uint16 sprNum, OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes sprT) :
    GameObject_common(pos), Sprite(sprNum, -1, sprT), OBox() {
    isActive = true;
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);
  }

  SpriteObject::SpriteObject(const SpriteObject & other) :
    GameObject_common(other), Sprite(other), OBox(other),
    
    objId(other.objId) {
    m_M = TranslateMatrix3D(pos);
    m_M.RotZ(rot);

    isActive = other.isActive;
  }

  void SpriteObject::update(Uint32 ticks) {
    anim.update(ticks);
  }

  Projectile::Projectile(unsigned char t, float r, Vector3D p, Vector3D d, uint32_t ticks, uint32_t o) :
    GameObject_common(p, r),
    typeId(t), delta(d), endsAtTick(ticks),
    owner(o), lastUpdateAt(ticks) {
    endsAtTick = lastUpdateAt + 1000;
  }

  Projectile::Projectile(const Projectile & other) :
    GameObject_common(other),
    typeId(other.typeId), delta(other.delta), endsAtTick(other.endsAtTick),
    owner(other.owner), lastUpdateAt(other.lastUpdateAt) {}

  void Projectile::update(uint32_t ticks) {
    Uint32 dt = ticks - lastUpdateAt;
    Vector3D new_pos(pos + delta * dt);
    /*INFO << "p-m " << pos.x << " " << pos.y << " " << pos.z << 
    " to " << new_pos.x << " " << new_pos.y << " " << new_pos.z << std::endl;
    */
    std::list<Pedestrian> & list = SpriteManagerHolder::Instance().getList<Pedestrian>();
    for (std::list<Pedestrian>::iterator i = list.begin(); i != list.end(); ++i) {
      Pedestrian & ped = *i;
      if (ped.id() == owner)
        continue;
      if (ped.isDead)
        continue;
      /*INFO << "ped " << ped.id() << " pos: " << ped.pos.x << " " << ped.pos.y << " " << ped.pos.z << std::endl;
      Vector3D p = ped.GetCenterPoint();
      INFO << "CP " << p.x << " " << p.y << " " << p.z << std::endl;
      p = ped.m_Extent;
      INFO << "extent " << p.x << " " << p.y << " " << p.z << std::endl;
      for (int i=0; i < 4; i++) {
        for (int j=0; j < 4; j++) {
          std::cout << "M " << ped.m_M.m[i][j] << " ";
        }
        std::cout << std::endl;
      }*/
        

      if (ped.IsLineInBox( pos, new_pos ) ) {
        INFO << "HIT ped " << ped.id() << std::endl;
        ped.getShot(true);
        //ped.sprType = GraphicsBase::SpriteNumbers::OBJECT;
        //ped.remap = -1;
        //SpriteManagerHolder::Instance().removePed(ped.pedId);
        endsAtTick = 0;
      }
    }
    
    /*
    Util::CellIterator oi(pos);

    Util::CellIterator ni(new_pos);
    //FIXME ni valid?
    
    if (oi.isValid() && ni.isValid()) {
      if (oi == ni) { // only one cell to check
        Math::line_intersect(pos, new_pos);//, oi.getBlock());
      }
      else { // crosses cell boundary
        Math::line_intersect(pos, new_pos);//, oi.getBlock());
        //Math::line_intersect(pos, new_pos);//, ni.getBlock());
      }
    }
    else
      INFO << "NEITHER VALID!"<< oi.x << " " << oi.y << " " << oi.z <<std::endl;
    */
    pos = new_pos; 

    lastUpdateAt = ticks;
  }
}
