#ifndef PARTICLE_H
#define PARTICLE_H

#include <stingray/core.h>

namespace stingray {
class Particle {
public:
  Vec3 position;
  Vec3 velocity;
  Vec3 acceleration;

  /* rough approx of drag to mititage inaccuracies that can cause strange
   * acceleration
   */
  real damping;

  void setInverseMass(const real value) { inverseMass = value; }
  void setMass(const real value) { inverseMass = 1 / value; }
  real getMass() const { return 1 / inverseMass; }
  real getInverseMass() const { return inverseMass; }
  void integrate(real duration);

protected:
  real inverseMass;
};

} // namespace stingray
#endif
