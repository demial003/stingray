#include <stingray/pfgen.h>
#include <stingray/plinks.h>
namespace stingray {
class ParticleWorld {
private:
  struct ParticleRegistration {
    Particle *particle;
    ParticleRegistration *next;
  };
  void startFrame();

  ParticleRegistration *firstParticle;

  bool calculateIterations;

  ParticleForceRegistry registry;

  ParticleContactResolver resolver;

  struct ContactGenRegistration {
    ParticleContactGenerator *gen;
    ContactGenRegistration *next;
  };

  ContactGenRegistration *firstContactGen;

  ParticleContact *contacts;

  unsigned maxContacts;

public:
  ParticleWorld(unsigned maxContacts, unsigned iterations = 0);
  unsigned generateContacts();
  void integrate(real duration);
  void runPhysics(real duration);
};
} // namespace stingray
