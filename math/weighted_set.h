#ifndef WEIGHTED_SET_H
#define WEIGHTED_SET_H

#include <vector>
#include "yasli/random.h"

namespace Math {

  class WeightedSet {
    public:
      WeightedSet(unsigned int seed);
      void add(unsigned int w, unsigned int e);
      unsigned int getRandom();
    private:
      std::vector<unsigned int> elements;
      Random                    rng;
  };
}

#endif
