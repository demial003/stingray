#include "particle.h"
#include <assert.h>
// #include <stingray/particle.h>

using namespace stingray;

void Particle::integrate(real duration) {
  assert(duration > 0.0);

  position.addScaledVec3(velocity, duration);
  Vec3 resultingAccel = acceleration;
  resultingAccel.addScaledVec3(forceAccum, inverseMass);

  velocity.addScaledVec3(resultingAccel, duration);

  velocity *= pow(damping, duration);
}
