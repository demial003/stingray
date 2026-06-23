#include <stingray/particle.h>
namespace stingray {

class ParticleContact {

  friend class ParticleContactResolver;

public:
  Particle *particle[2];
  real restitution;
  Vec3 contactNormal;
  real penetration;

protected:
  void resolve(real duration);
  real calculateSeparatingVelocity() const;
  void resolveInterpenetration(real duration);

private:
  void resolveVelocity(real duration);
};

class ParticleContactResolver {
protected:
  unsigned iterations;
  unsigned iterationsUsed;

public:
  ParticleContactResolver(unsigned iterations);
  void setIterations(unsigned iterations);
  void resolveContacts(ParticleContact *contactArray, unsigned numContacts,
                       real duration);
};

class ParticleContactGenerator {
public:
  virtual unsigned addContact(ParticleContact *contat,
                              unsigned limit) const = 0;
};
} // namespace stingray
