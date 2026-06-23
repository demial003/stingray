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

ParticleSpring::ParticleSpring(Particle *other, real springConstant,
                               real restLength)
    : other(other), springConstant(springConstant), restLength(restLength) {};

void ParticleSpring::updateForce(Particle *particle, real duration) {
  Vec3 force;
  particle->getPosition(&force);
  force -= other->getPosition();

  real magnitude = force.magnitude();
  magnitude = fabs(magnitude - restLength);
  magnitude *= springConstant;

  force.normalize();
  force *= -magnitude;
  particle->addForce(force);
}

ParticleAnchoredSpring::ParticleAnchoredSpring(Vec3 *anchor,
                                               real springConstant,
                                               real restLength)
    : anchor(anchor), springConstant(springConstant), restLength(restLength) {};
void ParticleAnchoredSpring::updateForce(Particle *particle, real duration) {
  Vec3 force;
  particle->getPosition(&force);
  force -= *anchor;

  real magnitude = force.magnitude();
  magnitude = fabs(magnitude - restLength);
  magnitude *= springConstant;

  force.normalize();
  force *= -magnitude;
  particle->addForce(force);
}

ParticleBungee::ParticleBungee(Particle *other, real springConstant,
                               real restLength)
    : other(other), springConstant(springConstant), restLength(restLength) {};

void ParticleBungee::updateForce(Particle *particle, real duration) {
  Vec3 force;
  particle->getPosition(&force);
  force -= other->getPosition();

  real magnitude = force.magnitude();
  if (magnitude <= restLength)
    return;

  force.normalize();
  force *= -magnitude;
  particle->addForce(force);
}

ParticleBuoyancy::ParticleBuoyancy(real maxDepth, real volume, real waterHeight,
                                   real liquidDensity)
    : maxDepth(maxDepth), volume(volume), waterHeight(waterHeight),
      liquidDensity(liquidDensity) {};
void ParticleBuoyancy::updateForce(Particle *particle, real duration) {
  real depth = particle->getPosition().y;

  if (depth >= waterHeight + maxDepth)
    return;

  Vec3 force(0, 0, 0);
  if (depth <= waterHeight - maxDepth) {
    force.y = liquidDensity * volume;
    particle->addForce(force);
    return;
  }
  force.y =
      liquidDensity * volume * (depth - maxDepth - waterHeight) / 2 * maxDepth;
  particle->addForce(force);
}
