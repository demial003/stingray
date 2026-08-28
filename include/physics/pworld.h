#pragma once

#include <physics/pfgen.h>
#include <physics/plinks.h>
namespace stingray {
namespace physics {
class ParticleWorld {
public:
  typedef std::vector<Particle *> Particles;
  typedef std::vector<ParticleContactGenerator *> ContactGenerators;

private:
  Particles particles;
  ContactGenerators contactGenerators;

  bool calculateIterations;

  ParticleForceRegistry registry;

  ParticleContactResolver resolver;

  ParticleContact *contacts;

  unsigned maxContacts;

public:
  ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
  ~ParticleWorld();
  void startFrame();
  unsigned generateContacts();
  ContactGenerators &getContactGenerators();
  Particles &getParticles();
  void integrate(real duration);
  void runPhysics(real duration);
};
} // namespace physics
} // namespace stingray
