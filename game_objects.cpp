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
#include "plane.h"
#include "ai.h"
#include "localplayer.h"

// ugly fix for win32
#ifdef WIN32
#undef ERROR
#define ERROR Util::Log::error(__FILE__, __LINE__)
#endif

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
    OpenGTA::Map & map = OpenGTA::ActiveMap::Instance().get();
    while (y >= map.getNumBlocksAtNew(x_b, z_b) && y > 0.0f)
      y -= 1.0f;
    while (y < map.getNumBlocksAtNew(x_b, z_b) && y > 0.0f) {
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(x_b, z_b, (PHYSFS_uint8)y);
      assert(block);
      if (block->blockType() > 0) {
        float bz = slope_height_offset(block->slopeType(), v.x - x, v.z - z);
        if (block->slopeType() == 0 && (block->blockType() != 5 &&
          block->blockType() != 6))
          bz -= 1.0f;
        //INFO << "hit " << int(block->blockType()) << " at " << int(y) << std::endl;
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
        if (block->slopeType() == 0 && (block->blockType() != 5
          && block->blockType() != 6))
          bz -= 1.0f;
        //INFO << "hit " << int(block->blockType()) << " at " << int(y) << std::endl;
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
    //anim(SpriteManager::Instance().getAnimationById(0)),
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
    anim = Animation(SpriteManager::Instance().getAnimationById(newId));
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::LOOP);
    animId = newId;
  }

  uint32_t Pedestrian::fistAmmo = 0;

  Pedestrian::Pedestrian(Vector3D e, const Vector3D & p, uint32_t id, Sint16 remapId) :
    GameObject_common(p), 
    Sprite(0, remapId, GraphicsBase::SpriteNumbers::PED), 
    OBox(TranslateMatrix3D(p), e * 0.5f),
    m_control(),
    speedForces(0, 0, 0), 
    inventory(), activeWeapon(0), activeAmmo(&fistAmmo),
    aiData() {
      m_M = TranslateMatrix3D(p);
      m_M.RotZ(-rot);
      pedId = id;
      animId = 0;
      isDead = 0;
      lastUpdateAt = Timer::Instance().getRealTime();
      inGroundContact = 0;
    }

  Pedestrian::Pedestrian(const Pedestrian & other) :
    GameObject_common(other), Sprite(other), OBox(other),

    pedId(other.pedId),
    m_control(),
    speedForces(other.speedForces),
    inventory(other.inventory),
    activeWeapon(other.activeWeapon), 
    activeAmmo(&inventory.find(activeWeapon)->second),
    aiData(other.aiData) {
      lastUpdateAt = other.lastUpdateAt; 
      inGroundContact = other.inGroundContact;
      animId = other.animId;
      isDead = other.isDead;
      m_M = TranslateMatrix3D(other.pos);
      m_M.RotZ(-other.rot);
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
      //AI::Pedestrian::walk_pavement(this);
    if (aiMode) {
      AI::Pedestrian::moveto_shortrange(this);
    }
    uint8_t chooseWeapon = m_control.getActiveWeapon(); 
    if (chooseWeapon != activeWeapon) {
      if (chooseWeapon == 0) {
        activeWeapon = 0;
        activeAmmo = &fistAmmo;
      }
      else {
        InventoryMap::iterator i = inventory.find(chooseWeapon);
        if (i != inventory.end()) {
          activeWeapon = chooseWeapon;
          activeAmmo = &i->second;
        }
      }
    }
activeWeapon = chooseWeapon;
    switch(m_control.getMove()) {
      case 1:
        if (m_control.getRunning()) {
          if (!(animId == 3u + activeWeapon*3))
            switchToAnim(3 + activeWeapon*3);
        }
        else {
          if (!(animId == 2u + activeWeapon*3))
            switchToAnim(2 + activeWeapon*3);
        }
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
    anim.update(ticks);
    Uint32 delta = ticks - lastUpdateAt;
    //INFO << "delta = " << delta  << " t: " << ticks << " lt: " << lastUpdateAt << std::endl;
    moveDelta = Vector3D(0, 0, 0);
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
    if (pedId == 0xffffffff) {
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
      SpriteManager::Instance().createProjectile(0, rot, pos, d1, ticks, pedId);
      lastWeaponTick = ticks;
    }
    /*
    if (m_control.statusChanged()) {
      INFO << "Ped-event id: " << pedId << " control: " << m_control.getRaw() <<
        " time: " << ticks << std::endl;
    }*/

    //INFO << pos.x << " " << pos.y << " " << pos.z << std::endl;
    lastUpdateAt = ticks;
  }

  void Pedestrian::tryMove(Vector3D nPos) {
    float x, y, z;
    x = floor(nPos.x);
    y = floor(nPos.y);
    z = floor(nPos.z);
    OpenGTA::Map & map = OpenGTA::ActiveMap::Instance().get();
    OpenGTA::GraphicsBase & graphics = OpenGTA::ActiveStyle::Instance().get();
    //INFO << heightOverTerrain(nPos) << std::endl;
    float hot = heightOverTerrain(nPos);
    if (hot > 0.3f)
      inGroundContact = 0;
    else if (hot < 0.0) {
      WARN << "gone below: " << hot << " at " << nPos.x << ", " << nPos.y << ", " << nPos.z << std::endl;
      nPos.y -= (hot - 0.3f);
      //nPos.y += 1;
      //INFO << nPos.y << std::endl;
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
      OpenGTA::Map::BlockInfo * block = map.getBlockAtNew(PHYSFS_uint8(x), PHYSFS_uint8(z), PHYSFS_uint8(y));
      assert(block);
      if (block->left && graphics.isBlockingSide(block->left)) {
        if (block->isFlat()) {
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
      if (block->top && graphics.isBlockingSide(block->top)) {
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
        if (block->left && graphics.isBlockingSide(block->left)) {
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
        if (block->top && graphics.isBlockingSide(block->top)) {
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
    bool obj_blocked = false;
    std::list<Car> & list = SpriteManager::Instance().getList<Car>();
    for (std::list<Car>::iterator i = list.begin(); i != list.end(); i++) {
      if (isBoxInBox(*i)) {
        if (Util::distance(pos, i->pos) > Util::distance(nPos, i->pos))
          obj_blocked = true;
      }
    }
    if ((inGroundContact) && (obj_blocked == false))
      pos = nPos;
    //else
    //  inGroundContact = 0;

  }

  void Pedestrian::die() {
    INFO << "DIE!!!" << std::endl; 
    switchToAnim(42);
    if (isDead == 3) {
      anim.set(Util::Animation::STOPPED, Util::Animation::STOP);
      return;
    }
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::FCALLBACK);
    anim.setCallback([this]() { die(); });
    isDead++;
  }

  void Pedestrian::getShot(uint32_t shooterId, uint32_t dmg, bool front) {
    isDead = 1;
    switchToAnim(45);
    anim.set(Util::Animation::PLAY_FORWARD, Util::Animation::FCALLBACK);
    anim.setCallback([this]() { die(); });
  }

  CarSprite::CarSprite() : sprNum(0), remap(-1), 
  sprType(GraphicsBase::SpriteNumbers::CAR), delta(0),
  deltaSet(sizeof(delta) * 8, (unsigned char*)&delta),
  animState() {}

  CarSprite::CarSprite(const CarSprite & o) :
    sprNum(o.sprNum), remap(o.remap), sprType(o.sprType), delta(o.delta),
    deltaSet(sizeof(delta) * 8, (unsigned char*)&delta),
    animState(o.animState) {}

  CarSprite::CarSprite(Uint16 sprN, Sint16 rem, 
      GraphicsBase::SpriteNumbers::SpriteTypes sprT) : sprNum(sprN),
  remap(rem), sprType(sprT), delta(0),
  deltaSet(sizeof(delta) * 8, (unsigned char*)&delta),
  animState() {}

  void CarSprite::setDamage(uint8_t k) {
    deltaSet.set_item(k, true);
  }

  bool CarSprite::assertDeltaById(uint8_t k) {
    GraphicsBase & style = ActiveStyle::Instance().get();
    PHYSFS_uint16 absNum = style.spriteNumbers.reIndex(sprNum, sprType);
    GraphicsBase::SpriteInfo * info = style.getSprite(absNum);
    if (k >= info->deltaCount)
      return false;
    return true;
  }

  void CarSprite::openDoor(uint8_t k) {
    DoorDeltaAnimation dda(k, true);
    doorAnims.push_back(dda);
  }

  void CarSprite::closeDoor(uint8_t k) {
    doorAnims.push_back(DoorDeltaAnimation(k, false));
  }

  void CarSprite::setSirenAnim(bool on) {
    if (!(assertDeltaById(15) && assertDeltaById(16))) {
      ERROR << "Trying to set siren anim on car-sprite that has no such delta!" << std::endl;
      return;
    }
    animState.set_item(10, on);
  }

  void CarSprite::update(Uint32 ticks) {

// siren anim indices
#define DSI_1 15
#define DSI_2 16

#define D_A_THEN_B(d, a, b) (d.get_item(a)) { d.set_item(a, false); d.set_item(b, true); }

    // drive-anim
    if (animState.get_item(0)) {}
    /*
    if (ticks - lt_door > 500) {
      // 1-4 door-opening
      if (animState.get_item(1)) {
        deltaSet.set_item(6, true);
      }
      // 5-8 door-closing
      lt_door = ticks;
    }*/
    DoorAnimList::iterator i = doorAnims.begin();
    while (i != doorAnims.end()) {
      i->update(ticks);
      if (i->opening) {
        if (i->doorId == 0) {
          for (int k=6; k < 10; k++)
            deltaSet.set_item(k, false);
          deltaSet.set_item(i->getCurrentFrameNumber() + 6, true);
        }
        else if (i->doorId == 1) {
          for (int k=11; k < 15; k++)
            deltaSet.set_item(k, false);
          deltaSet.set_item(i->getCurrentFrameNumber() + 11, true);
        }
        else if (i->doorId == 2) {
          for (int k=20; k < 24; k++)
            deltaSet.set_item(k, false);
          deltaSet.set_item(i->getCurrentFrameNumber() + 20, true);
        }
        else if (i->doorId == 3) {
          for (int k=24; k < 28; k++)
            deltaSet.set_item(k, false);
          deltaSet.set_item(i->getCurrentFrameNumber() + 24, true);
        }
      }
      else {
        if (i->doorId == 0) {
          for (int k=6; k < 10; k++)
            deltaSet.set_item(k, false);
          if (i->getCurrentFrameNumber() > 0)
            deltaSet.set_item(i->getCurrentFrameNumber() + 5, true);
        }
        else if (i->doorId == 1) {
          for (int k=11; k < 15; k++)
            deltaSet.set_item(k, false);
          if (i->getCurrentFrameNumber() > 0)
            deltaSet.set_item(i->getCurrentFrameNumber() + 10, true);
        }
        else if (i->doorId == 2) {
          for (int k=20; k < 24; k++)
            deltaSet.set_item(k, false);
          if (i->getCurrentFrameNumber() > 0)
            deltaSet.set_item(i->getCurrentFrameNumber() + 19, true);
        }
        else if (i->doorId == 3) {
          for (int k=24; k < 28; k++)
            deltaSet.set_item(k, false);
          if (i->getCurrentFrameNumber() > 0)
            deltaSet.set_item(i->getCurrentFrameNumber() + 23, true);
        }
      }
      if (i->get() == Util::Animation::STOPPED) {
        DoorAnimList::iterator j = i;
        i++;
        animState.set_item(j->doorId + 1, j->opening);
        doorAnims.erase(j);
      }
      else
        i++;
    }
    if (animState.get_item(10)) {
      if (ticks - lt_siren > 500) {
        if D_A_THEN_B(deltaSet, DSI_1, DSI_2)
        else if D_A_THEN_B(deltaSet, DSI_2, DSI_1)
        else { deltaSet.set_item(DSI_1, true); deltaSet.set_item(DSI_2, false); }
        lt_siren = ticks;
      }
    }
  }

  CarSprite::DoorDeltaAnimation::DoorDeltaAnimation(uint8_t dId, bool dOpen) :
    Util::Animation(4 + (dOpen ? 0 : 1), 5), doorId(dId), opening(dOpen) {
    if (!opening) {
      set(Util::Animation::PLAY_BACKWARD, Util::Animation::STOP);
      jumpToFrame(4, Util::Animation::PLAY_BACKWARD);
    }
    else {
      set(Util::Animation::PLAY_FORWARD, Util::Animation::STOP);
    }
  }

  Car::Car(Vector3D & _pos, float _rot, uint32_t id, uint8_t _type, int16_t _remap) :
    GameObject_common(_pos, _rot),
    CarSprite(0, -1, GraphicsBase::SpriteNumbers::CAR), OBox(),
    carInfo(*ActiveStyle::Instance().get().findCarByModel(_type)) {
      type = _type;
      carId = id;
      sprNum = carInfo.sprNum;
      if ((_remap > -1) && (ActiveStyle::Instance().get().getFormat() == 0))
        remap = carInfo.remap8[_remap];
      fixSpriteType();
      m_Extent = Vector3D(INT2F_DIV128(carInfo.width),
          INT2F_DIV128(carInfo.depth),
          INT2F_DIV128(carInfo.height));
      m_M = TranslateMatrix3D(pos);
      m_M.RotZ(-rot);
      hitPoints = carInfo.damagable;
    }

  void Car::fixSpriteType() {
    if (carInfo.vtype == 3)
      sprType = GraphicsBase::SpriteNumbers::BIKE;
    else if (carInfo.vtype == 0)
      sprType = GraphicsBase::SpriteNumbers::BUS;
    else if (carInfo.vtype == 8)
      sprType = GraphicsBase::SpriteNumbers::TRAIN;
  }

  Car::Car(OpenGTA::Map::ObjectPosition& op, uint32_t id) :
    GameObject_common(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    CarSprite(0, -1, GraphicsBase::SpriteNumbers::CAR), OBox(),
    carInfo(*ActiveStyle::Instance().get().findCarByModel(op.type)){
      carId = id;
      type = op.type;
      if (op.remap - 128 > 0) {
        if (ActiveStyle::Instance().get().getFormat() == 0)
          remap = carInfo.remap8[op.remap-129];
        else
          WARN << "remap " << int(op.remap-129) << 
          " requested but not implemented for G24" << std::endl;
      }
      sprNum = carInfo.sprNum;
      fixSpriteType();
      m_Extent = Vector3D(INT2F_DIV128(carInfo.width),
          INT2F_DIV128(carInfo.depth) ,
          INT2F_DIV128(carInfo.height));
      m_M = TranslateMatrix3D(pos);

      rot = op.rotation * 360 / 1024;
      m_M.RotZ(-rot);
      hitPoints = carInfo.damagable;
    }

  Car::Car(const Car & other) :
    GameObject_common(other), CarSprite(other), OBox(other),
    carInfo(*ActiveStyle::Instance().get().findCarByModel(other.type)) {
      type = other.type;
      m_M = TranslateMatrix3D(pos);
      m_M.RotZ(-rot);
      hitPoints = other.hitPoints;

      carId = other.carId;
    }

  void Car::update(Uint32 ticks) {
    //m_M = TranslateMatrix3D(pos);
    //m_M.RotZ(rot+90);
    CarSprite::update(ticks);
  }

  void Car::damageAt(const Vector3D & hit, uint32_t dmg) {
    float angle = Util::xz_angle(Vector3D(0, 0, 0), hit);
    INFO << "hit angle: " << angle << std::endl;

    /*
     * front   rear 
     *       270
     * 0 _|o---o| _ 180
     *    |o---o|
     * 45    90
     */
#define A1 60
#define A2 (180 - A1)
#define A3 (180 + A1)
#define A4 (360 - A1)

    uint8_t k = 0;
    if (angle <= A1) {
      // front left
      k = 3;
    }
    else if (angle <= A2) {
      // left side
      k = 2;
    }
    else if (angle < 180.0f) {
      // rear left
      k = 1;
    }
    else if (angle <= A3) {
      // rear right
      k = 4;
    }
    else if (angle <= A4) {
      // right side
      k = 5;
    }
    else {
      // right front
      k = 0;
    }
    setDamage(k);
    hitPoints -= dmg;
    if (hitPoints <= 0)
      explode();
  }

  void Car::explode() {
    //SpriteManager::Instance().removeCar(carId);
    //return;
    Vector3D exp_pos(pos);
    exp_pos.y += 0.1f;
    SpriteManager::Instance().createExplosion(exp_pos);
    sprNum = 0;
    remap = -1;
    sprType = GraphicsBase::SpriteNumbers::WCAR;
    delta = 0;
  }

  SpriteObject::SpriteObject(OpenGTA::Map::ObjectPosition& op, uint32_t id) :
    GameObject_common(Vector3D(INT2FLOAT_WRLD(op.x), 6.05f-INT2FLOAT_WRLD(op.z), INT2FLOAT_WRLD(op.y))),
    Sprite(0, -1, GraphicsBase::SpriteNumbers::OBJECT), OBox() {
      objId = id;
      GraphicsBase & style = ActiveStyle::Instance().get();
      sprNum = style.objectInfos[op.type]->sprNum;
      m_Extent = Vector3D(INT2F_DIV128(style.objectInfos[op.type]->width),
          INT2F_DIV128(style.objectInfos[op.type]->depth),
          INT2F_DIV128(style.objectInfos[op.type]->height));
      m_M = TranslateMatrix3D(pos);
      m_M.RotZ(-rot);
      rot = op.rotation * 360 / 1024;
      isActive = true;
    }

  SpriteObject::SpriteObject(Vector3D pos, Uint16 sprNum, OpenGTA::GraphicsBase::SpriteNumbers::SpriteTypes sprT) :
    GameObject_common(pos), Sprite(sprNum, -1, sprT), OBox() {
      isActive = true;
      m_M = TranslateMatrix3D(pos);
      m_M.RotZ(-rot);
    }

  SpriteObject::SpriteObject(const SpriteObject & other) :
    GameObject_common(other), Sprite(other), OBox(other),

    objId(other.objId) {
      m_M = TranslateMatrix3D(pos);
      m_M.RotZ(-rot);

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

  bool Projectile::testCollideBlock_flat(Util::CellIterator & ci, Vector3D & newp) {
    Map::BlockInfo & bi = ci.getBlock();
    if (bi.top) {
      Math::Plane plane(Vector3D(ci.x, ci.z, ci.y), Vector3D(0, 0, -1));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect flat-t: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.x >= ci.x && hit_pos.x <= ci.x + 1) {
          newp = hit_pos;
          return true;
        }
      }
    }
    if (bi.left) {
      Math::Plane plane(Vector3D(ci.x, ci.z, ci.y), Vector3D(-1, 0, 0));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect flat-l: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.z >= ci.y && hit_pos.z <= ci.y + 1) {
          newp = hit_pos;
          return true;
        }
      }
    }
    return false;
  }

  bool Projectile::testCollideBlock(Util::CellIterator & ci, Vector3D & newp) {
    Map::BlockInfo & bi = ci.getBlock();
    //INFO << "pos: " << ci.x << " " << ci.y << " " << ci.z << std::endl;
    //if (bi.isFlat())
    //  return false;
    if (bi.left) {
      Math::Plane plane(Vector3D(ci.x, ci.z, ci.y), Vector3D(-1, 0, 0));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect left: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.z >= ci.y && hit_pos.z <= ci.y + 1) {
          newp = hit_pos;
          return true;
        }
      }
    }
    if (bi.right && !bi.isFlat()) {
      Math::Plane plane(Vector3D(ci.x+1, ci.z, ci.y), Vector3D(1, 0, 0));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect right: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.z >= ci.y && hit_pos.z <= ci.y + 1) {
          newp = hit_pos;
          return true;
        }
      }
    }
    if (bi.top) {
      Math::Plane plane(Vector3D(ci.x, ci.z, ci.y), Vector3D(0, 0, -1));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect top: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.x >= ci.x && hit_pos.x <= ci.x + 1) {
          newp = hit_pos;
          return true;
        }
      }
    }
    if (bi.bottom && !bi.isFlat()) {
      Math::Plane plane(Vector3D(ci.x, ci.z, ci.y+1), Vector3D(0, 0, 1));
      Vector3D hit_pos;
      if (plane.segmentIntersect(pos, newp, hit_pos)) {
        INFO << "intersect bottom: " << hit_pos.x << " " << hit_pos.y << " " <<hit_pos.z << std::endl; 
        if (hit_pos.x >= ci.x && hit_pos.x <= ci.x + 1) {
          newp = hit_pos;
          return true;
        }
      }

    }
    return false;
  }

  void Projectile::update(uint32_t ticks) {
    Uint32 dt = ticks - lastUpdateAt;
    Vector3D new_pos(pos + delta * dt);
    /*INFO << "p-m " << pos.x << " " << pos.y << " " << pos.z << 
      " to " << new_pos.x << " " << new_pos.y << " " << new_pos.z << std::endl;
      */
    std::list<Pedestrian> & list = SpriteManager::Instance().getList<Pedestrian>();
    for (std::list<Pedestrian>::iterator i = list.begin(); i != list.end(); ++i) {
      Pedestrian & ped = *i;
      if (ped.id() == owner)
        continue;
      if (ped.isDead)
        continue;

      if (ped.isLineInBox( pos, new_pos ) ) {
        Vector3D p;
        ped.lineCrossBox(pos, new_pos, p);
        float angle = Util::xz_angle(Vector3D(0,0,0), p);
        INFO << angle << std::endl;
        if (angle <= 90.0f || angle > 270.0f)
          INFO << "FRONT" << std::endl;
        else
          INFO << "BACK" << std::endl;
        ped.getShot(owner, Projectile::damageByType(typeId), true);
        LocalPlayer & pc = LocalPlayer::Instance();
        if (owner == pc.getId()) {
          pc.addCash(10);
          pc.addWanted(1);
        }
        endsAtTick = 0;
      }
    }
    std::list<Car> & clist = SpriteManager::Instance().getList<Car>();
    for (std::list<Car>::iterator i = clist.begin(); i != clist.end(); i++) {
      Car & car = *i;

      if (car.isLineInBox(pos, new_pos)) {
        INFO << "CAR HIT" << std::endl;
        Vector3D p;
        car.lineCrossBox(pos, new_pos, p);
        car.damageAt(p, 5);
        //INFO << Util::xz_angle(Vector3D(0,0,0), p) << std::endl;
        delta = Vector3D(0, 0, 0);
        p = Transform(p, car.m_M);
        new_pos.x = p.x;
        new_pos.z = p.z;
        new_pos.y += 0.1f;
      }
    }

    Util::CellIterator oi(pos);
    int collided = 0;
    if (oi.isValid()) {
      Map::BlockInfo & bi = oi.getBlock();
      if (bi.isFlat()) {
        collided += testCollideBlock_flat(oi, new_pos);
      }
      Util::CellIterator ni(oi.right());
      if (ni.isValid())
        collided += testCollideBlock(ni, new_pos);
      ni = oi.left();
      if (ni.isValid())
        collided += testCollideBlock(ni, new_pos);
      ni = oi.top();
      if (ni.isValid())
        collided += testCollideBlock(ni, new_pos);
      ni = oi.bottom();
      if (ni.isValid())
        collided += testCollideBlock(ni, new_pos);
    }
    if (collided)
      delta = Vector3D(0, 0, 0);
    pos = new_pos; 

    lastUpdateAt = ticks;
  }

  uint32_t Projectile::damageByType(const uint8_t & k) {
    uint32_t v = 7;
    switch(k) {
      case 2:
        v = 150;
        break;
    }
    return v;
  }
}
