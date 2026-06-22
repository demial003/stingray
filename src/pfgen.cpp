#include <stingray/pfgen.h>
using namespace stingray;

void ParticleForceRegistry::add(Particle *particle,
                                ParticleForceGenerator *fg) {
  registrations.push_back({particle, fg});
}

void ParticleForceRegistry::updateForces(real duration) {
  Registry::iterator i = registrations.begin();
  for (; i != registrations.end(); i++) {
    i->fg->updateForce(i->particle, duration);
  }
}

ParticleGravity::ParticleGravity(const Vec3 &gravity) : gravity(gravity) {}
void ParticleGravity::updateForce(Particle *particle, real duration) {
  if (!particle->hasFiniteMass())
    return;

  particle->addForce(gravity * particle->getMass());
}

ParticleDrag::ParticleDrag(real k1, real k2) : k1(k1), k2(k2) {}

void ParticleDrag::updateForce(Particle *particle, real duration) {
  Vec3 force;
  particle->getVelocity(&force);

  real dragCoeff = force.magnitude();
  dragCoeff = k1 * dragCoeff + k2 * dragCoeff * dragCoeff;
  force.normalize();
  force *= -dragCoeff;
  particle->addForce(force);
}
