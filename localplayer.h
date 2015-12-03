#ifndef OGTA_LOCAL_PLAYER_H
#define OGTA_LOCAL_PLAYER_H
#include "Singleton.h"
#include "game_objects.h"
#include "entity_controller.h"
#include "id_sys.h"
#include "key_handler.h"

namespace OpenGTA {

  class PlayerController : public Util::KeyHandler {
    public:
      PlayerController() {
        reset();
      }
      void reset() {
        playerId = TypeIdBlackBox::getPlayerId();
        cash = 0;
        wantedLevel = 0;
        modifier = 0;
        numLives = 0;
        pc_ptr = NULL;
      }
      PedController & getCtrl() {
        assert(pc_ptr);
        return *pc_ptr;
      }
      void setCtrl(PedController & pc) {
        pc_ptr = &pc;
      }
      void giveLives(uint16_t k) {
        numLives += k;
      }
      void disableCtrl(bool soft);
      void enableCtrl();
      Pedestrian & getPed();
      int32_t  getNumLives() { return numLives; }
      int32_t  getWantedLevel() { return wantedLevel; }
      uint32_t getCash() { return cash; }
      bool up(const uint32_t & key);
      bool down(const uint32_t & key);
      uint32_t getId() { return playerId; }
      void addCash(uint32_t v) { cash += v; }
      void setWanted(int32_t v) { wantedLevel = v; }
      void addWanted(uint32_t v) { wantedLevel += v; if (wantedLevel > 5) wantedLevel = 5; }
    private:
      uint32_t playerId;
      uint32_t cash;
      int32_t  wantedLevel;
      uint32_t modifier;
      int32_t  numLives;
      PedController * pc_ptr;
  };

  typedef Loki::SingletonHolder<PlayerController, Loki::CreateUsingNew,
    Loki::DefaultLifetime, Loki::SingleThreaded> LocalPlayer;
}

#endif
