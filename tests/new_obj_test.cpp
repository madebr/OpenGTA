#include "log.h"
#include "game_objects.h"
#include "abstract_container.h"

using namespace OpenGTA;

class ConcreteContainer : 
  public AbstractContainer<Pedestrian>,
  public AbstractContainer<Car> {
  public:
    template <typename T> void add(const T & t) {
      AbstractContainer<T>::doAdd(t);
    }
    Car & getCar(uint32_t id) {
      return AbstractContainer<Car>::doGet(id);
    }
    Pedestrian & getPed(uint32_t id) {
      return AbstractContainer<Pedestrian>::doGet(id);
    }
    void removeCar(uint32_t id) {
      AbstractContainer<Car>::doRemove(id);
    }
    void removePed(uint32_t id) {
      AbstractContainer<Pedestrian>::doRemove(id);
    }
    void realRemove() {
      AbstractContainer<Car>::doRealRemove();
      AbstractContainer<Pedestrian>::doRealRemove();
    }
};

int main(int argc, char* argv[]) {

  Car c1(1);

  Pedestrian p1(20);
  Car c2(20);

  ConcreteContainer cc;
  cc.add(p1);
  cc.add(c1);
  cc.add(c2);

  Car & c = cc.getCar(1);
  INFO << c.carId << std::endl;

  Pedestrian & p = cc.getPed(20);
  cc.removeCar(20);
  cc.realRemove();
  c = cc.getCar(20);
}
