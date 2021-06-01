/************************************************************************
* Copyright (c) 2005-2007 tok@openlinux.org.uk                          *
*                                                                       *
* This file contains code derived from information copyrighted by       *
* DMA Design. It may not be used in a commercial product.               *
*                                                                       *
* See license.txt for details.                                          *
*                                                                       *
* This notice may not be removed or altered.                            *
************************************************************************/
#include <iostream>
#include <cassert>
#include "opengta.h"
#include "buffercache.h"
#include "file_helper.h"
#include "log.h"
#include "m_exceptions.h"
#include "set.h"

using namespace Util;

namespace OpenGTA {
  
#define GTA_GRAPHICS_GRX 290
#define GTA_GRAPHICS_GRY 325
#define GTA_GRAPHICS_G24 336

  PHYSFS_uint16 GraphicsBase::SpriteNumbers::countByType(const SpriteTypes & t) const {
    switch (t) {
      case ARROW:
        return GTA_SPRITE_ARROW;
      case DIGIT:
        return GTA_SPRITE_DIGITS;
      case BOAT:
        return GTA_SPRITE_BOAT;
      case BOX:
        return GTA_SPRITE_BOX;
      case BUS:
        return GTA_SPRITE_BUS;
      case CAR:
        return GTA_SPRITE_CAR;
      case OBJECT:
        return GTA_SPRITE_OBJECT;
      case PED:
        return GTA_SPRITE_PED;
      case SPEEDO:
        return GTA_SPRITE_SPEEDO;
      case TANK:
        return GTA_SPRITE_TANK;
      case TRAFFIC_LIGHT:
        return GTA_SPRITE_TRAFFIC_LIGHTS;
      case TRAIN:
        return GTA_SPRITE_TRAIN;
      case TRDOOR:
        return GTA_SPRITE_TRDOORS;
      case BIKE:
        return GTA_SPRITE_BIKE;
      case TRAM:
        return GTA_SPRITE_TRAM;
      case WBUS:
        return GTA_SPRITE_WBUS;
      case WCAR:
        return GTA_SPRITE_WCAR;
      case EX:
        return GTA_SPRITE_EX;
      case TUMCAR:
        return GTA_SPRITE_TUMCAR;
      case TUMTRUCK:
        return GTA_SPRITE_TUMTRUCK;
      case FERRY:
        return GTA_SPRITE_FERRY;
      default:
        break;
    }
    INFO << "UPS: " << t << std::endl;
    assert(0);
  }
  PHYSFS_uint16 GraphicsBase::SpriteNumbers::reIndex(const PHYSFS_uint16 & id, const SpriteTypes & t) const {
    switch (t) {
      case ARROW:
        return id;
      case DIGIT:
        return GTA_SPRITE_ARROW + id;
      case BOAT:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + id;
      case BOX:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + id;
      case BUS:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + id;
      case CAR:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + id;
      case OBJECT:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + id;
      case PED:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + id;
      case SPEEDO:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + id;
      case TANK:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + id;
      case TRAFFIC_LIGHT:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + id;
      case TRAIN:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + id;
      case TRDOOR:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + id;
      case BIKE:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + id;
      case TRAM:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + id;
      case WBUS:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + id;
      case WCAR:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + GTA_SPRITE_WBUS + id;
      case EX:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + GTA_SPRITE_WBUS + GTA_SPRITE_WCAR + id;
      case TUMCAR:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + GTA_SPRITE_WBUS + GTA_SPRITE_WCAR + 
          GTA_SPRITE_TUMCAR + id;
      case TUMTRUCK:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + GTA_SPRITE_WBUS + GTA_SPRITE_WCAR + 
          GTA_SPRITE_TUMCAR + GTA_SPRITE_TUMCAR + id;
      case FERRY:
        return GTA_SPRITE_ARROW + GTA_SPRITE_DIGITS + GTA_SPRITE_BOAT + 
          GTA_SPRITE_BOX + GTA_SPRITE_BUS + GTA_SPRITE_CAR + GTA_SPRITE_OBJECT
          + GTA_SPRITE_PED + GTA_SPRITE_SPEEDO + GTA_SPRITE_TANK + 
          + GTA_SPRITE_TRAFFIC_LIGHTS + GTA_SPRITE_TRAIN + GTA_SPRITE_TRDOORS + 
          GTA_SPRITE_BIKE + GTA_SPRITE_TRAM + GTA_SPRITE_WBUS + GTA_SPRITE_WCAR + 
          GTA_SPRITE_TUMCAR + GTA_SPRITE_TUMCAR + GTA_SPRITE_TUMTRUCK + id;
    }
    assert(0); // should never be reached
    return 0;
  }

  GraphicsBase::GraphicsBase() : sideTexBlockMove(256) {
    rawTiles = NULL;
    rawSprites = NULL;
    delta_is_a_set = false;
    for (int i=0; i < 256; ++i)
      sideTexBlockMove.set_item(i, true);
  }

  bool GraphicsBase::isBlockingSide(uint8_t id) {
    return sideTexBlockMove.get_item(id);
  }

  void GraphicsBase::setupBlocking(const std::string & filename) {
    // style001
    sideTexBlockMove.set_item(10, false);
    sideTexBlockMove.set_item(20, false);
    sideTexBlockMove.set_item(97, false);
    sideTexBlockMove.set_item(109, false);
    sideTexBlockMove.set_item(110, false);
    sideTexBlockMove.set_item(115, false);
    sideTexBlockMove.set_item(116, false);
    sideTexBlockMove.set_item(155, false);
    sideTexBlockMove.set_item(156, false);
    sideTexBlockMove.set_item(157, false);
    sideTexBlockMove.set_item(158, false);

  }

  bool GraphicsBase::getDeltaHandling() {
    return delta_is_a_set;
  }

  void GraphicsBase::setDeltaHandling(bool delta_as_set) {
    delta_is_a_set = delta_as_set;
  }

  GraphicsBase::~GraphicsBase() {
    if (fd)
      PHYSFS_close(fd);
    std::vector<LoadedAnim*>::iterator i = animations.begin();
    while (i != animations.end()) {
      delete *i;
      i++;
    }
    animations.clear();
    std::vector<SpriteInfo*>::iterator i2 = spriteInfos.begin();
    while (i2 != spriteInfos.end()) {
      delete *i2;
      i2++;
    }
    spriteInfos.clear();
    std::vector<ObjectInfo*>::iterator i3 = objectInfos.begin();
    while (i3 != objectInfos.end()) {
      delete *i3;
      i3++;
    }
    objectInfos.clear();
    std::vector<CarInfo*>::iterator i4 = carInfos.begin();
    while (i4 != carInfos.end()) {
      delete *i4;
      i4++;
    }
    carInfos.clear();
    if (rawTiles)
      delete [] rawTiles;
    if (rawSprites)
      delete [] rawSprites;
  }

  bool GraphicsBase::isAnimatedBlock(uint8_t area_code, uint8_t id) {
    std::vector<LoadedAnim*>::iterator i = animations.begin();
    while (i != animations.end()) {
      if ((*i)->which == area_code && (*i)->block == id)
        return true;
      ++i;
    }
    return false;
  }

  GraphicsBase::CarInfo* GraphicsBase::findCarByModel(PHYSFS_uint8 model) {
    std::vector<CarInfo*>::iterator i = carInfos.begin();
    while (i != carInfos.end()) {
      if ((*i)->model == model)
        return *i;
      ++i;
    }
    //throw std::string("Failed to find car by model");
    throw E_UNKNOWNKEY("Searching for car model " + std::to_string(int(model))
                       + " failed");
  }

  unsigned int GraphicsBase::getRandomPedRemapNumber() {
    return int(rand() * (1.0f / (1.0f + RAND_MAX)) * 
      (lastValidPedRemap - firstValidPedRemap) +
        firstValidPedRemap);
  }

  unsigned int GraphicsBase::getPedRemapNumberType(unsigned int _type) {
    ERROR << "not implemented"<< std::endl;
    return _type;
  }
  
  uint8_t GraphicsBase::getFormat() {
    if (_topHeaderSize == 52)
      return 0;
    else if (_topHeaderSize == 64)
      return 1;
    throw E_INVALIDFORMAT("graphics-base header size");
    return 255;
  }    

  Graphics8Bit::Graphics8Bit(const std::string& style) : GraphicsBase() {
    fd = Util::FileHelper::OpenReadVFS(style);
    _topHeaderSize = 52;
    rawTiles = NULL;
    rawSprites = NULL;
    masterRGB = NULL;
    auxBlockTrailSize = 0;
    loadHeader();
    setupBlocking(style);
    firstValidPedRemap = 131;
    lastValidPedRemap = 187;
  }

  Graphics8Bit::~Graphics8Bit() {
    //if (rawSprites)
    //  delete [] rawSprites;
    if (masterRGB)
      delete masterRGB;
  }
  
  void Graphics8Bit::dump() {
    
    uint32_t gs = sideSize + lidSize + auxSize;

    INFO << "* graphics info *" << std::endl <<
    gs << " bytes in " << gs / 65536 << " pages " << gs / 4096 
    << " images" << std::endl <<
    spriteInfos.size() << " sprites (" << spriteInfoSize <<") total: " << 
    spriteGraphicsSize << " bytes" << std::endl <<
    "sprite numbers: " << std::endl <<
    spriteNumbers.GTA_SPRITE_ARROW << " arrows" << std::endl <<
    spriteNumbers.GTA_SPRITE_DIGITS << " digits" << std::endl <<
    spriteNumbers.GTA_SPRITE_BOAT << " boats" << std::endl <<
    spriteNumbers.GTA_SPRITE_BOX << " boxes" << std::endl <<
    spriteNumbers.GTA_SPRITE_BUS << " buses" << std::endl <<
    spriteNumbers.GTA_SPRITE_CAR << " cars" << std::endl <<
    spriteNumbers.GTA_SPRITE_OBJECT << " objects" << std::endl <<
    spriteNumbers.GTA_SPRITE_PED << " peds" << std::endl <<
    spriteNumbers.GTA_SPRITE_SPEEDO << " speedos" << std::endl <<
    spriteNumbers.GTA_SPRITE_TANK << " tanks" << std::endl <<
    spriteNumbers.GTA_SPRITE_TRAFFIC_LIGHTS << " traffic lights" << std::endl <<
    spriteNumbers.GTA_SPRITE_TRAIN << " trains" << std::endl <<
    spriteNumbers.GTA_SPRITE_TRDOORS << " train doors" << std::endl <<
    spriteNumbers.GTA_SPRITE_BIKE << " bikes" << std::endl <<
    spriteNumbers.GTA_SPRITE_TRAM << " trams" << std::endl <<
    spriteNumbers.GTA_SPRITE_WBUS << " wbuses" << std::endl <<
    spriteNumbers.GTA_SPRITE_WCAR << " wcars" << std::endl <<
    spriteNumbers.GTA_SPRITE_EX << " exes"<< std::endl <<
    spriteNumbers.GTA_SPRITE_TUMCAR << " tumcars" << std::endl <<
    spriteNumbers.GTA_SPRITE_TUMTRUCK << " tumtrucks" << std::endl <<
    spriteNumbers.GTA_SPRITE_FERRY << " ferries"<< std::endl <<
    "#object-info: " << objectInfos.size() << " #car-info: " << carInfos.size() << std::endl;
  }

  void Graphics8Bit::loadHeader() {
    PHYSFS_uint32 vc;
    PHYSFS_readULE32(fd, &vc);
    if(vc != GTA_GRAPHICS_GRY) {
      ERROR << "graphics file specifies version " << vc <<
        " instead of " << GTA_GRAPHICS_GRY << std::endl;
      throw E_INVALIDFORMAT("8-bit loader failed");
      return;
    }
    PHYSFS_readULE32(fd, &sideSize);
    PHYSFS_readULE32(fd, &lidSize);
    PHYSFS_readULE32(fd, &auxSize);
    PHYSFS_readULE32(fd, &animSize);
    PHYSFS_readULE32(fd, &paletteSize);
    PHYSFS_readULE32(fd, &remapSize);
    PHYSFS_readULE32(fd, &remapIndexSize);
    PHYSFS_readULE32(fd, &objectInfoSize);
    PHYSFS_readULE32(fd, &carInfoSize);
    PHYSFS_readULE32(fd, &spriteInfoSize);
    PHYSFS_readULE32(fd, &spriteGraphicsSize);
    PHYSFS_readULE32(fd, &spriteNumberSize);
    
    INFO << "Block textures: S " << sideSize / 4096 << " L " <<
      lidSize / 4096 << " A " << auxSize / 4096 << std::endl;
    if (sideSize % 4096 != 0) {
      ERROR << "Side-Block texture size is not a multiple of 4096" << std::endl;
      return;
    }
    if (lidSize % 4096 != 0) {
      ERROR << "Lid-Block texture size is not a multiple of 4096" << std::endl;
      return;
    }
    if (auxSize % 4096 != 0) {
      ERROR << "Aux-Block texture size is not a multiple of 4096" << std::endl;
      return;
    }
    
    PHYSFS_uint32 tmp = sideSize / 4096 + lidSize / 4096 + auxSize / 4096;
    tmp = tmp % 4;
    if (tmp) {
      auxBlockTrailSize = (4 - tmp) * 4096;
      INFO << "adjusting aux-block by " << auxBlockTrailSize << std::endl;
    }
    INFO << "Anim size: " << animSize << " palette size: " << paletteSize << 
      " remap size: " << remapSize << " remap-index size: " << remapIndexSize << std::endl;
    INFO << "Obj-info size: " << objectInfoSize << " car-size: " << carInfoSize <<
      " sprite-info size: " << spriteInfoSize << " graphic size: " << spriteGraphicsSize <<
      " numbers s: " << spriteNumberSize << std::endl;
    if (spriteNumberSize != 42) {
      ERROR << "spriteNumberSize is " << spriteNumberSize << " (should be 42)" << std::endl;
      return;
    }
    loadTileTextures();
    loadAnim();
    loadPalette();
    loadRemapTables();
    loadRemapIndex();
    loadObjectInfo();
    loadCarInfo();
    loadSpriteInfo();
    loadSpriteGraphics();
    loadSpriteNumbers();
    dump();
  }

  void GraphicsBase::loadAnim() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize;
    PHYSFS_seek(fd, st);
    PHYSFS_uint8 numAnim;
    PHYSFS_readBytes(fd, static_cast<void*>(&numAnim), 1);
    for (int i=0; i<numAnim;i++) {
      uint8_t block, which, speed, frameCount;
      PHYSFS_readBytes(fd, static_cast<void*>(&block), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&which), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&speed), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&frameCount), 1);
      LoadedAnim *animation = new LoadedAnim(frameCount);
      animation->block = block;
      animation->which = which;
      animation->speed = speed;
      animation->frameCount = frameCount;
      
      if (animation->frameCount > 180) {
        ERROR << "found animation with " << int(animation->frameCount) 
          << " frames ???" << std::endl;
      }
      for (int j=0; j<animation->frameCount; j++) {
        uint8_t val;
        PHYSFS_readBytes(fd, static_cast<void*>(&val), 1);
        animation->frame[j] = val;
        //PHYSFS_read(fd, static_cast<void*>(&animations[i].frame[j]), 1, 1);
      }
      animations.push_back(animation);
    }
  }
  
  void Graphics8Bit::loadPalette() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize;
    PHYSFS_seek(fd, st);
    if (masterRGB)
      delete masterRGB;
    masterRGB = new RGBPalette(fd);
  }
  
  void Graphics8Bit::loadRemapTables() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize;
    PHYSFS_seek(fd, st);
    PHYSFS_readBytes(fd, static_cast<void*>(remapTables), sizeof(remapTables));
    /*
    for (int i=0; i < 256; i++) {
      for (int j = 0; j < 256; j++) {
        std::cout << int(remapTables[i][j]) << " ";
      }
      std::cout << std::endl;
    }*/
  }

  void Graphics8Bit::loadRemapIndex() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize;
    PHYSFS_seek(fd, st);
    PHYSFS_readBytes(fd, static_cast<void*>(remapIndex), sizeof(remapIndex));
    /*
    std::cout << "LID remap tables" << std::endl;
    for (int i=0; i<256; ++i) {
      std::cout << i << ": " << int(remapIndex[i][0]) << ", " << int(remapIndex[i][1]) <<
        ", " << int(remapIndex[i][2]) << ", " << int(remapIndex[i][3]) << std::endl;
    }*/
  }
  
  void GraphicsBase::loadObjectInfo_shared(PHYSFS_uint64 offset) {
    PHYSFS_seek(fd, offset);
    assert(objectInfoSize % 20 == 0);
    int c = objectInfoSize / 20;

    for (int i=0; i< c; i++) {
      ObjectInfo *obj = new ObjectInfo();
      PHYSFS_readULE32(fd, &obj->width);
      PHYSFS_readULE32(fd, &obj->height);
      PHYSFS_readULE32(fd, &obj->depth);
      PHYSFS_readULE16(fd, &obj->sprNum);
      PHYSFS_readULE16(fd, &obj->weight);
      PHYSFS_readULE16(fd, &obj->aux);
      PHYSFS_readBytes(fd, static_cast<void*>(&obj->status), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&obj->numInto), 1);

      objectInfos.push_back(obj);
    }
    
  }

  void Graphics8Bit::loadObjectInfo() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize;
    loadObjectInfo_shared(st);
  }

  void GraphicsBase::loadCarInfo_shared(PHYSFS_uint64 offset) {
    PHYSFS_seek(fd, offset);

    //INFO << "starting at offset " << offset << std::endl;
    PHYSFS_uint32 bytes_read = 0;
    while (bytes_read < carInfoSize) {
      //INFO << bytes_read << ": " << carInfoSize << std::endl;
      CarInfo * car = new CarInfo();

      PHYSFS_readSLE16(fd, &car->width);
      PHYSFS_readSLE16(fd, &car->height);
      PHYSFS_readSLE16(fd, &car->depth);
      PHYSFS_readSLE16(fd, &car->sprNum);
      PHYSFS_readSLE16(fd, &car->weightDescriptor);
      PHYSFS_readSLE16(fd, &car->maxSpeed);
      PHYSFS_readSLE16(fd, &car->minSpeed);
      PHYSFS_readSLE16(fd, &car->acceleration);
      PHYSFS_readSLE16(fd, &car->braking);
      PHYSFS_readSLE16(fd, &car->grip);
      PHYSFS_readSLE16(fd, &car->handling);
      bytes_read += 2 * 11;

      for (int i=0; i < 12; i++) {
        PHYSFS_readSLE16(fd, &car->remap24[i].h);
        PHYSFS_readSLE16(fd, &car->remap24[i].l);
        PHYSFS_readSLE16(fd, &car->remap24[i].s);
      }
      bytes_read += 12 * 3 * 2;
      for (int i=0; i < 12; i++) {
        PHYSFS_readBytes(fd, static_cast<void*>(&car->remap8[i]), 1);
      }
      bytes_read += 12;

      PHYSFS_readBytes(fd, static_cast<void*>(&car->vtype), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->model), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->turning), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->damagable), 1);
      bytes_read += 4;

      for (int i=0; i < 4; i++)
        PHYSFS_readULE16(fd, &car->value[i]);
      bytes_read += 4 * 2;

      PHYSFS_readBytes(fd, static_cast<void*>(&car->cx), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->cy), 1);
      PHYSFS_readULE32(fd, &car->moment);
      bytes_read += 2 + 4;

      PHYSFS_uint32 fixed_tmp;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->rbpMass = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->g1_Thrust = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->tyreAdhesionX = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->tyreAdhesionY = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->handBrakeFriction = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->footBrakeFriction = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->frontBrakeBias = float(fixed_tmp)/65536;
      bytes_read += 7 * 4;

      PHYSFS_readSLE16(fd, &car->turnRatio);
      PHYSFS_readSLE16(fd, &car->driveWheelOffset);
      PHYSFS_readSLE16(fd, &car->steeringWheelOffset);
      bytes_read += 3 * 2;

      PHYSFS_readULE32(fd, &fixed_tmp);
      car->backEndSlideValue = float(fixed_tmp)/65536;
      PHYSFS_readULE32(fd, &fixed_tmp);
      car->handBrakeSlideValue = float(fixed_tmp)/65536;
      bytes_read += 2 * 4;

      PHYSFS_readBytes(fd, static_cast<void*>(&car->convertible), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->engine), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->radio), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->horn), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->soundFunction), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&car->fastChangeFlag), 1);
      bytes_read += 6;

      PHYSFS_readSLE16(fd, &car->numDoors);
      bytes_read += 2;
      if (car->numDoors > 2) {
        WARN << "num-doors: " << car->numDoors << " > 2 ???" << std::endl;
        car->numDoors = 0;
      }

      for (int i=0; i < car->numDoors; i++) {
        PHYSFS_readSLE16(fd, &car->door[i].rpy);
        PHYSFS_readSLE16(fd, &car->door[i].rpx);
        PHYSFS_readSLE16(fd, &car->door[i].object);
        PHYSFS_readSLE16(fd, &car->door[i].delta);
        bytes_read += 4 * 2;
      }
      carInfos.push_back(car);

    }
    assert(bytes_read == carInfoSize);

  }

  void Graphics8Bit::loadCarInfo() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize + objectInfoSize;
    loadCarInfo_shared(st);
  }
  
  void Graphics8Bit::loadSpriteInfo() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize + objectInfoSize + carInfoSize;
    PHYSFS_seek(fd, st);

    PHYSFS_uint8 v;
    PHYSFS_uint32 w;
    PHYSFS_uint32 _bytes_read = 0;
    while (_bytes_read < spriteInfoSize) {
      SpriteInfo *si = new SpriteInfo();
      PHYSFS_readBytes(fd, static_cast<void*>(&si->w), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&si->h), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&si->deltaCount), 1);
      PHYSFS_readBytes(fd, static_cast<void*>(&v), 1);
      PHYSFS_readULE16(fd, &si->size);
      PHYSFS_readULE32(fd, &w);
      _bytes_read += 10;
      //si->ptr = reinterpret_cast<unsigned char*>(w);
      si->page = w / 65536;
      si->xoffset = (w % 65536) % 256;
      si->yoffset = (w % 65536) / 256;
      si->clut = 0;

      /*
      std::cout << int(si->w) << "," << int(si->h) << " = " << si->size << " deltas: " << int(si->deltaCount) <<
        " at " << w << std::endl;
      */

      // sanity check
      if (v)
        WARN << "Compression flag active in sprite!" << std::endl;
      if (int(si->w) * int(si->h) != int(si->size)) {
        ERROR << "Sprite info size mismatch: " << int(si->w) << "x" << int(si->h) <<
          " != " << si->size << std::endl;
        return;
      }
      if (si->deltaCount > 32) {
        ERROR << "Delta count of sprite is " << si->deltaCount << std::endl;
        return;
      }
      for (PHYSFS_uint8 j = 0; j < 33; ++j) {
        si->delta[j].size = 0;
        si->delta[j].ptr = 0;
        if (si->deltaCount && (j < si->deltaCount)) {
          //std::cout << "reading " << int(j) << std::endl;
          PHYSFS_readULE16(fd, &si->delta[j].size);
          PHYSFS_readULE32(fd, &w);
          _bytes_read += 6;
          si->delta[j].ptr = reinterpret_cast<unsigned char*>(w);
        }
      }
      spriteInfos.push_back(si);
      
    }
    st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize + objectInfoSize + carInfoSize + spriteInfoSize;
    assert(PHYSFS_tell(fd) == PHYSFS_sint64(st));
  }

  void Graphics8Bit::loadSpriteGraphics() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize + objectInfoSize + carInfoSize + spriteInfoSize;
    PHYSFS_seek(fd, st);
    rawSprites = new unsigned char[spriteGraphicsSize];
    assert(rawSprites != NULL);
    PHYSFS_readBytes(fd, static_cast<void*>(rawSprites), spriteGraphicsSize);

    if (spriteInfos.size() == 0) {
      INFO << "No SpriteInfo post-loading work done - structure is empty" << std::endl;
      return;
    }
    std::vector<SpriteInfo*>::const_iterator i = spriteInfos.begin();
    std::vector<SpriteInfo*>::const_iterator end = spriteInfos.end();
    PHYSFS_uint32 _pagewise = 256 * 256;
    while (i != end) {
      SpriteInfo *info = *i;
      /*
      PHYSFS_uint32 offset = reinterpret_cast<PHYSFS_uint32>(info->ptr);
      PHYSFS_uint32 page = offset / 65536;
      PHYSFS_uint32 y = (offset % 65536) / 256;
      PHYSFS_uint32 x = (offset % 65536) % 256;
      */
      //std::cout << int(info->w) << "x" << int(info->h) << " " << int(info->deltaCount) << " deltas" << std::endl;
      //std::cout << offset << " page " << page << " x,y " << x <<","<<y<< std::endl;
//info->ptr = rawSprites + page * _pagewise + 256 * y + x;
      for (uint8_t k = 0; k < info->deltaCount; ++k) {
        const auto tmp = reinterpret_cast<uintptr_t>(info->delta[k].ptr);
        const auto offset = static_cast<PHYSFS_uint32>(tmp);
        const auto page = offset / 65536;
        const auto y = (offset % 65536) / 256;
        const auto x = (offset % 65536) % 256;
        info->delta[k].ptr = rawSprites + page * _pagewise + 256 * y + x;
      }
      i++;
    }

    return;
  }

  void GraphicsBase::loadSpriteNumbers_shared(PHYSFS_uint64 offset) {
  
    PHYSFS_seek(fd, offset);
    
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_ARROW);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_DIGITS);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_BOAT);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_BOX);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_BUS);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_CAR);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_OBJECT);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_PED);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_SPEEDO);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TANK);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TRAFFIC_LIGHTS);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TRAIN);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TRDOORS);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_BIKE);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TRAM);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_WBUS);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_WCAR);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_EX);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TUMCAR);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_TUMTRUCK);
    PHYSFS_readULE16(fd, &spriteNumbers.GTA_SPRITE_FERRY);
    
  }

  void Graphics8Bit::loadSpriteNumbers() {
    PHYSFS_uint64 st = static_cast<PHYSFS_uint64>(_topHeaderSize) +
      sideSize + lidSize + auxSize + auxBlockTrailSize + animSize + paletteSize +
      remapSize + remapIndexSize + objectInfoSize + carInfoSize +
      spriteInfoSize + spriteGraphicsSize;
    loadSpriteNumbers_shared(st);
  }

  void GraphicsBase::loadTileTextures() {
    PHYSFS_seek(fd, static_cast<PHYSFS_uint64>(_topHeaderSize));

    PHYSFS_uint64 ts = sideSize+lidSize+auxSize;
    rawTiles = new unsigned char[ts];
    int r = PHYSFS_readBytes(fd, static_cast<void*>(rawTiles), ts);
    if ( PHYSFS_uint64(r) == ts )
        return;
    else if ( r == -1) {
       INFO << "Could not read texture raw data" << std::endl;
       return;
    }
    else
      INFO << "This message should never be displayed! (" << std::endl;
  }

  void GraphicsBase::handleDeltas(const SpriteInfo & info, unsigned char* buffer, Uint32 delta) {
    const unsigned int b_offset = 256 * info.yoffset + info.xoffset;
    if (delta_is_a_set) {
      Util::Set delta_set(32, (unsigned char*)&delta);
      for (int i = 0; i < 20; ++i) {
        if (delta_set.get_item(i)) {
          assert(i < info.deltaCount);
          const DeltaInfo & di = info.delta[i];
          applyDelta(info, buffer, b_offset, di);
        }
      }
      for (int i=20; i < 24; i++) {
        if (delta_set.get_item(i)) {
          const DeltaInfo & di = info.delta[i - 20 + 6];
          applyDelta(info, buffer, b_offset, di, true);
        }
      }
      for (int i=24; i < 28; i++) {
        if (delta_set.get_item(i)) {
          const DeltaInfo & di = info.delta[i - 24 + 11];
          applyDelta(info, buffer, b_offset, di, true);
        }
      }
      //assert(0);
    }
    else {
      // delta is only an index; one to big
      assert(delta <= info.deltaCount);
      const DeltaInfo & di = info.delta[delta - 1];
      applyDelta(info, buffer, b_offset, di);
    }
  }

  unsigned char* Graphics8Bit::getSpriteBitmap(size_t id, int remap = -1, Uint32 delta = 0) {
    SpriteInfo *info = spriteInfos[id];
    assert(info != NULL);
    //PHYSFS_uint32 offset = reinterpret_cast<PHYSFS_uint32>(info->ptr);
    //const PHYSFS_uint32 page = offset / 65536;
    const PHYSFS_uint32 y = info->yoffset; // (offset % 65536) / 256;
    const PHYSFS_uint32 x = info->xoffset; // (offset % 65536) % 256;
    const PHYSFS_uint32 page_size = 256 * 256;

    unsigned char * page_start = rawSprites + info->page * page_size;// + 256 * y + x;
    assert(page_start != NULL);
    
    BufferCache & bcache = BufferCache::Instance();
    unsigned char * dest = bcache.requestBuffer(page_size);
    bcache.lockBuffer(dest);

    unsigned char * result = dest;
    assert(dest != NULL);
    memcpy(dest, page_start, page_size);
    if (delta > 0) {
      handleDeltas(*info, result, delta);
      /*
      assert(delta < info->deltaCount);
      DeltaInfo & di = info->delta[delta];
      applyDelta(*info, result+256*y+x, di);
      */
    }
    if (remap > -1)
      applyRemap(page_size, remap, result);
    unsigned char* bigbuf = bcache.requestBuffer(page_size * 4);
    
    masterRGB->apply(page_size, result, bigbuf, true);
    assert(page_size > PHYSFS_uint32(info->w * info->h * 4));
    for (uint16_t i = 0; i < info->h; i++) {
      memcpy(result, bigbuf+(256*y+x)*4, info->w * 4);
      result += info->w * 4;
      bigbuf += 256 * 4;
    }

    return dest;
  } 

  void GraphicsBase::applyDelta(const SpriteInfo & spriteInfo, unsigned 
      char* buffer, Uint32 page_offset, const DeltaInfo & deltaInfo, bool mirror) {
    unsigned char* b = buffer + page_offset;
    unsigned char* delta = deltaInfo.ptr;
    PHYSFS_sint32 length_to_go = deltaInfo.size;

    if (mirror) {
      PHYSFS_uint32 doff = 0;
      while (length_to_go > 0) {
        PHYSFS_uint16* offset = (PHYSFS_uint16*) delta;
        doff += *offset;
        delta += 2;
        unsigned char this_length = *delta;
        ++delta;
        PHYSFS_uint32 noff = page_offset + doff;
        PHYSFS_uint32 _y = noff / 256 * 256;
        PHYSFS_uint32 _x = doff % 256;
        for (int i=0; i < this_length; i++)
          *(buffer + _y + spriteInfo.xoffset + spriteInfo.w - _x - i - 1) = *(delta+i);
        length_to_go -= (this_length + 3);
        doff += this_length;
        delta += this_length;
      }
      return;
    }

    while (length_to_go > 0) {
      PHYSFS_uint16* offset = (PHYSFS_uint16*) delta;
      b += *offset;
      delta += 2;
      unsigned char this_length = *delta;
      ++delta;
      memcpy(b, delta, this_length);
      b += this_length;
      delta += this_length;
      length_to_go -= (this_length + 3);
    }

  }

  void Graphics8Bit::applyRemap(unsigned int len, unsigned int which, unsigned char* buffer) {
    assert(buffer!=NULL);
    unsigned char* t = buffer;
    for (unsigned int i = 0; i < len; ++i) {
      *t = remapTables[which][*t]; //FIXME: is this the right order? Is this correct at all?
      t++;
    }
  }
  
  void GraphicsBase::prepareSideTexture(unsigned int idx, unsigned char* dst) {
    assert(dst!=NULL);
    ++idx;
    assert(rawTiles);
    unsigned char* rt = rawTiles + (idx / 4) * 4096 * 4 + (idx % 4) * 64;
    for (int i=0; i<64; i++) {
      memcpy(dst, rt, 64);
      dst += 64;
      rt += 64*4;
    }
  }
  
  unsigned char* Graphics8Bit::getSide(unsigned int idx, unsigned int palIdx, bool rgba = false) {
    prepareSideTexture(idx-1, tileTmp);
    unsigned char *res;
    if (rgba) {
      masterRGB->apply(4096, tileTmp, tileTmpRGBA, true);
      res = tileTmpRGBA;
    }
    else {
      masterRGB->apply(4096, tileTmp, tileTmpRGB, false);
      res = tileTmpRGB;
    }
    return res;
  }

  void GraphicsBase::prepareLidTexture(unsigned int idx, unsigned char* dst) {
    assert(dst!=NULL);
    unsigned char* rt = rawTiles;
    assert(rawTiles);
    idx += sideSize / 4096 + 1; // FIXME: assumes partition == block end
    rt += (idx / 4) * 4096 * 4 + (idx % 4) * 64;
    for (int i=0; i<64; i++) {
      memcpy(dst, rt, 64);
      dst += 64;
      rt += 64*4;
    }
  }
  
  unsigned char *Graphics8Bit::getLid(unsigned int idx, unsigned int palIdx, bool rgba = false) {
    prepareLidTexture(idx-1, tileTmp);
    if (palIdx > 0)
      applyRemap(4096, palIdx, tileTmp);
    
    unsigned char *res;
    if (rgba) {
      masterRGB->apply(4096, tileTmp, tileTmpRGBA, true);
      res = tileTmpRGBA;
    }
    else {
      masterRGB->apply(4096, tileTmp, tileTmpRGB, false);
      res = tileTmpRGB;
    }
    return res;
  }
  
  void GraphicsBase::prepareAuxTexture(unsigned int idx, unsigned char* dst) {
    assert(dst!=NULL);
    unsigned char* rt = rawTiles;
    assert(rawTiles);
    idx += (sideSize+lidSize)/4096 + 1; // FIXME: assumes partition == block end
    rt += (idx / 4) * 4096 * 4 + (idx % 4) * 64;
    for (int i=0; i<64; i++) {
      memcpy(dst, rt, 64);
      dst += 64;
      rt += 64*4;
    }
  }
  
  unsigned char* Graphics8Bit::getAux(unsigned int idx, unsigned int palIdx, bool rgba = false) {
    prepareAuxTexture(idx-1, tileTmp);
    unsigned char *res;
    if (rgba) {

      masterRGB->apply(4096, tileTmp, tileTmpRGBA, true);
      res = tileTmpRGBA;
    }
    else {
      masterRGB->apply(4096, tileTmp, tileTmpRGB, false);
      res = tileTmpRGB;
    }
    return res;
  }

  unsigned char* GraphicsBase::getTmpBuffer(bool rgba = false) {
    if (rgba)
      return tileTmpRGBA;
    return tileTmpRGB;
  }
        
  /* RGBPalette */
  Graphics8Bit::RGBPalette::RGBPalette() {
  }
  
  Graphics8Bit::RGBPalette::RGBPalette(const std::string& palette) {
    PHYSFS_file* fd = Util::FileHelper::OpenReadVFS(palette);
    loadFromFile(fd);
  }
  
  Graphics8Bit::RGBPalette::RGBPalette(PHYSFS_file* fd) {
    loadFromFile(fd);
  }
  
  int Graphics8Bit::RGBPalette::loadFromFile(PHYSFS_file* fd) {
    PHYSFS_readBytes(fd, static_cast<void*>(data), sizeof(data));
    /*
    int max_sum = 0;
    for (int i = 1; i < 256; i+=3) {
      int sum = int(data[i]) + int(data[i+1]) + int(data[i+2]);
      if (sum > max_sum)
        max_sum = sum;
    }*/
    return 0;
  }
  
  void Graphics8Bit::RGBPalette::apply(unsigned int len, const unsigned char* src,
      unsigned char* dst, bool rgba ) {
    for (unsigned int i = 0; i < len; i++) {
      *dst = data[*src * 3 ]; ++dst;
      *dst = data[*src * 3 + 1]; ++dst;
      *dst = data[*src * 3 + 2]; ++dst;
      if (rgba) {
        if (*src == 0)
          *dst = 0x00; 
        else
          *dst = 0xff;
        ++dst;
      }
      ++src;
    }
  }
}
