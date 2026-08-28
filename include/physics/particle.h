#pragma once

#include <physics/core.h>

namespace stingray {
namespace physics {
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
  Vec3 getVelocity() const;
  void getPosition(Vec3 *dest) const;
  Vec3 getPosition() const;
  void getAcceleration(Vec3 *dest) const;
  Vec3 getAcceleration() const;
  void setVelocity(const Vec3 &velocity);
  void setVelocity(real x, real y, real z);
  void setPosition(const Vec3 &position);
  void setPosition(real x, real y, real z);
  void setAcceleration(const Vec3 &acceleration);
  void setAcceleration(real x, real y, real z);
  real getMass() const { return 1 / inverseMass; }
  real getInverseMass() const { return inverseMass; }
  bool hasFiniteMass() const;
  void integrate(real duration);
  void clearAccumulator();
  void addForce(const Vec3 &force);

protected:
  real inverseMass;
};

} // namespace physics
} // namespace stingray
