#include <stingray/pcontacts.h>

namespace stingray {
class ParticleLink : public ParticleContactGenerator {
public:
  Particle *particle[2];
  virtual unsigned addContact(ParticleContact *contact,
                              unsigned limit) const = 0;

protected:
  real currentLength() const;
};

class ParticleCable : public ParticleLink {
public:
  real maxLength;
  real restitution;
  virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;
};

class ParticleRod : public ParticleLink {
public:
  real length;
  virtual unsigned addContact(ParticleContact *contact, unsigned limit) const;
};
} // namespace stingray
