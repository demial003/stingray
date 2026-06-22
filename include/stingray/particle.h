#ifndef PARTICLE_H
#define PARTICLE_H

#include <stingray/core.h>

namespace stingray {
class Particle {
public:
  Vec3 position;
  Vec3 velocity;
  Vec3 acceleration;
  Vec3 forceAccum;

  /* rough approx of drag to mititage inaccuracies that can cause strange
   * acceleration
   */
  real damping;

  void setInverseMass(const real value) { inverseMass = value; }
  void setMass(const real value) { inverseMass = 1 / value; }
  void getVelocity(Vec3 *dest) const;
  void setVelocity(const Vec3 &velocity);
  void setVelocity(real x, real y, real z);
  real getMass() const { return 1 / inverseMass; }
  real getInverseMass() const { return inverseMass; }
  bool hasFiniteMass() const;
  void integrate(real duration);
  void clearAccumulator();
  void addForce(const Vec3 &force);

protected:
  real inverseMass;
};

} // namespace stingray
#endif
