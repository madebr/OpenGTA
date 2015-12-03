#ifndef OGTA_LOCAL_PLAYER_H
#define OGTA_LOCAL_PLAYER_H
#include "Singleton.h"
#include "game_objects.h"
#include "entity_controller.h"
#include "id_sys.h"

namespace OpenGTA {

  class PlayerController { // : public PedController { //public Pedestrian::Controller {
    public:
      PlayerController() {
        playerId = TypeIdBlackBox::getPlayerId();
        pc_ptr = NULL;
      }
      PedController & getCtrl() {
        assert(pc_ptr);
        return *pc_ptr;
      }
      void setCtrl(PedController & pc) {
        pc_ptr = &pc;
      }
    private:
      uint32_t playerId;
      PedController * pc_ptr;
  };

  typedef Loki::SingletonHolder<PlayerController, Loki::CreateUsingNew,
    Loki::DefaultLifetime, Loki::SingleThreaded> LocalPlayer;
}

#endif
