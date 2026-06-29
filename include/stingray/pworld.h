#pragma once

#ifndef PWORLD_H
#define PWORLD_H

#include <stingray/pfgen.h>
#include <stingray/plinks.h>
namespace stingray {
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
  void integrate(real duration);
  void runPhysics(real duration);
};
} // namespace stingray
#endif
