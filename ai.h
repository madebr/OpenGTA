#ifndef OGTA_AI_H
#define OGTA_AI_H

namespace OpenGTA {
  class Pedestrian;

  namespace AI {
    namespace Pedestrian {
      void walk_pavement(OpenGTA::Pedestrian*);
      void moveto_shortrange(OpenGTA::Pedestrian*);
      void move_away(OpenGTA::Pedestrian*);
    }
  }
}

#endif
