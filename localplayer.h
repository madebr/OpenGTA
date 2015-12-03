#ifndef OGTA_LOCAL_PLAYER_H
#define OGTA_LOCAL_PLAYER_H
#include "Singleton.h"
#include "pedestrian.h"

namespace OpenGTA {

  class PlayerController : public Pedestrian::Controller {
    public:
      PlayerController() { turn = 0; move = 0; }
  };

  typedef Loki::SingletonHolder<PlayerController, Loki::CreateUsingNew,
    Loki::DefaultLifetime, Loki::SingleThreaded> LocalPlayer;
}

#endif
