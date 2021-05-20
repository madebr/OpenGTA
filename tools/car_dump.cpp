#include <cassert>
#include <iostream>
#include <string>
#include "opengta.h"
#include "dataholder.h"

extern int global_EC;
extern int global_Done;
std::string map_file("undefined_map_file");
std::string style_file("undefined_style_file");
std::string msg_file("ENGLISH.FXT");

void on_exit() {
  PHYSFS_deinit();
  if (global_EC)
    std::cerr << "Exiting after fatal problem - please see output above" << std::endl;
}

void parse_args(int argc, char* argv[]) {
#ifdef DUMP_CARS_IN_MAP
  if (argc != 3) {
    std::cout << "USAGE: " << argv[0] << " map-file style-file" << std::endl;
    exit(1);
  }
  map_file = argv[1];
  style_file = argv[2];
#else
  if (argc != 2) {
    std::cout << "USAGE: " << argv[0] << " style-file" << std::endl;
    exit(1);
  }
  map_file = "";
  style_file = argv[1];
#endif

}

void print_car(OpenGTA::GraphicsBase::CarInfo & ci) {
  std::string model = "car" + std::to_string(int(ci.model));
#define PRINT(c)  << #c << ":" << ci.c << "|"
#define PRINTC(c) << #c << ":" << int(ci.c) << "|"
  std::cout  PRINT(width)  PRINT(height)  PRINT(depth)
  PRINT(sprNum) PRINT(weightDescriptor) PRINT(maxSpeed) PRINT(minSpeed)
  PRINT(acceleration) PRINT(braking) PRINT(grip) PRINT(handling)
  // remaps
  PRINTC(vtype) PRINTC(model) PRINTC(turning) PRINTC(damagable) <<
  "model-name:" << OpenGTA::MainMsgLookup::Instance().get().getText(model) << "|"
  PRINTC(cx)  PRINTC(cy)  PRINT(moment) 
  PRINT(rbpMass) PRINT(g1_Thrust) PRINT(tyreAdhesionX) PRINT(tyreAdhesionY)
  PRINT(handBrakeFriction) PRINT(footBrakeFriction) PRINT(frontBrakeBias)
  PRINT(turnRatio) PRINT(driveWheelOffset) PRINT(steeringWheelOffset)
  PRINT(backEndSlideValue) PRINT(handBrakeSlideValue)
  PRINTC(convertible) PRINTC(engine) PRINTC(radio) PRINTC(horn) PRINTC(soundFunction)
  PRINTC(fastChangeFlag) PRINT(numDoors);
#undef PRINT
#define PRINT(c) << #c << ":" << ci.door[i].c << "|"
  for (int i = 0; i < ci.numDoors; i++) {
    std::cout PRINT(rpx) PRINT(rpy) PRINT(object) PRINT(delta);
  }
  std::cout << "remap8:";
  for (int i = 0 ; i < 12; i++) {
    std::cout << int(ci.remap8[i]) << (i < 11 ? "," : "");
  }
  std::cout << std::endl;
}

void run_init(const char*) {
  PHYSFS_init("mapview");
  PHYSFS_mount(PHYSFS_getBaseDir(), nullptr, 1);
  PHYSFS_mount("gtadata.zip", nullptr, 1);
}

// dump all cars in style
void run_main() {
  OpenGTA::MainMsgLookup::Instance().load(msg_file);
  OpenGTA::ActiveStyle::Instance().load(style_file);
  OpenGTA::GraphicsBase & style = OpenGTA::ActiveStyle::Instance().get();
  std::cout << "DUMP_OBJ_INFO BEGIN" << std::endl;
  for (size_t i = 0; i < style.carInfos.size(); ++i) {
    OpenGTA::GraphicsBase::CarInfo * cinfo = style.carInfos[i];
    assert(cinfo);
    print_car(*cinfo);
  }

}
