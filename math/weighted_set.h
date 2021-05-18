#ifndef WEIGHTED_SET_H
#define WEIGHTED_SET_H

#include <random>
#include <vector>

namespace Math {

  class WeightedSet {
    public:
      WeightedSet(unsigned int seed);
      void add(unsigned int w, unsigned int e);
      unsigned int getRandom();
    private:
      std::vector<unsigned int> elements;
      std::mt19937 rng_;
  };
}

#endif
