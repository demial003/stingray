#include <stingray/pworld.h>
using namespace stingray;

ParticleWorld::ParticleWorld(unsigned maxContacts, unsigned iterations)
    : resolver(iterations), maxContacts(maxContacts) {
  contacts = new ParticleContact[maxContacts];
  calculateIterations = (iterations == 0);
}

ParticleWorld::~ParticleWorld() { delete[] contacts; }

void ParticleWorld::startFrame() {
  for (Particles::iterator p = particles.begin(); p != particles.end(); p++) {
    (*p)->clearAccumulator();
  }
}
unsigned ParticleWorld::generateContacts() {
  unsigned limit = maxContacts;
  ParticleContact *nextContact = contacts;
  for (ContactGenerators::iterator g = contactGenerators.begin();
       g != contactGenerators.end(); g++) {
    unsigned used = (*g)->addContact(nextContact, limit);
    limit -= used;
    nextContact += used;

    if (limit <= 0)
      break;
  }
  return maxContacts - limit;
}

void ParticleWorld::integrate(real duration) {
  for (Particles::iterator p = particles.begin(); p != particles.end(); p++) {
    (*p)->integrate(duration);
  }
}

void ParticleWorld::runPhysics(real duration) {
  registry.updateForces(duration);
  integrate(duration);
  unsigned usedContacts = generateContacts();
  if (calculateIterations)
    resolver.setIterations(usedContacts * 2);
  resolver.resolveContacts(contacts, usedContacts, duration);
}

ParticleWorld::ContactGenerators &ParticleWorld::getContactGenerators() {
  return contactGenerators;
}
