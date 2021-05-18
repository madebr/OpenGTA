#include "weighted_set.h"

namespace Math {
  WeightedSet::WeightedSet(unsigned int seed) :
    elements(),
    rng_{seed} {
    }

  void WeightedSet::add(unsigned int w, unsigned int e) {
    while (w > 0) {
      elements.push_back(e);
      --w;
    }
  }

  unsigned int WeightedSet::getRandom() {
    //unsigned int rnd = (int) (totalWeight * (rand() / (RAND_MAX + 1.0)));
    std::uniform_int_distribution<unsigned> distrib { 0, elements.size() };
    return elements[distrib(rng_)];
  }
}

/*
#include <iostream>
int main(int argc, char* argv[]) {
  Util::Math::WeightedSet set(int(getpid()) ^ int(time(0)));
  set.add(1, 1);
  set.add(2, 2);
  set.add(2, 3);
  set.add(3, 4);
  set.add(3, 5);

  std::cout << set.getRandom() << std::endl;
}*/
