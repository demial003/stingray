

#include <assert.h>
#include <stingray/particle.h>

using namespace stingray;

void Particle::integrate(real duration) {
  assert(duration > 0.0);

  position.addScaledVec3(velocity, duration);
  Vec3 resultingAccel = acceleration;
  resultingAccel.addScaledVec3(forceAccum, inverseMass);

  velocity.addScaledVec3(resultingAccel, duration);

  velocity *= pow(damping, duration);

  clearAccumulator();
}

void Particle::clearAccumulator() { forceAccum.clear(); }

void Particle::addForce(const Vec3 &force) { forceAccum += force; }

bool Particle::hasFiniteMass() const { return inverseMass >= 0.0f; }

void Particle::getVelocity(Vec3 *dest) const { *dest = Particle::velocity; }
void Particle::setVelocity(real x, real y, real z) {
  Particle::velocity.x = x;
  Particle::velocity.y = y;
  Particle::velocity.z = z;
}

void Particle::setVelocity(const Vec3 &velocity) {
  Particle::velocity = velocity;
}
