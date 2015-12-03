#include "id_sys.h"

namespace OpenGTA {
  uint32_t TypeIdBlackBox::nextId = 0;
  uint32_t TypeIdBlackBox::firstPlayerId = 0xffffffff - 32;
  uint32_t TypeIdBlackBox::lastPlayerId = 0xffffffff;

}
