

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
Vec3 Particle::getVelocity() const { return Particle::velocity; }
void Particle::setVelocity(real x, real y, real z) {
  Particle::velocity.x = x;
  Particle::velocity.y = y;
  Particle::velocity.z = z;
}

void Particle::setVelocity(const Vec3 &velocity) {
  Particle::velocity = velocity;
}

void Particle::setPosition(real x, real y, real z) {
  Particle::position.x = x;
  Particle::position.y = y;
  Particle::position.z = z;
}

void Particle::setPosition(const Vec3 &position) {
  Particle::position = position;
}
Vec3 Particle::getPosition() const { return Particle::position; }
void Particle::getPosition(Vec3 *dest) const { *(dest) = Particle::position; }

void Particle::setAcceleration(real x, real y, real z) {
  Particle::acceleration.x = x;
  Particle::acceleration.y = y;
  Particle::acceleration.z = z;
}

void Particle::setAcceleration(const Vec3 &acceleration) {
  Particle::acceleration = acceleration;
}
Vec3 Particle::getAcceleration() const { return Particle::acceleration; }
void Particle::getAcceleration(Vec3 *dest) const {
  *(dest) = Particle::acceleration;
}
