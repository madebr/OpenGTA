#ifndef ID_COUNTER_SYS_H
#define ID_COUNTER_SYS_H

#include "game_objects.h"
#include "m_exceptions.h"

namespace OpenGTA {
  
  class TypeIdBlackBox {
    public:
      static uint32_t requestId() {
        if (nextId + 1 >= firstPlayerId) {
          throw E_OUTOFRANGE("Player id range reached!");
        }
        return nextId++;
      }
      static uint32_t getPlayerId() {
        return lastPlayerId;
      }
    private:
      static uint32_t nextId;
      static uint32_t firstPlayerId;
      static uint32_t lastPlayerId;
    
  };

}

#endif
