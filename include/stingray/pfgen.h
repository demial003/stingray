#include <stingray/particle.h>
#include <vector>

namespace stingray {

class ParticleForceGenerator {
public:
  virtual void updateForce(Particle *particle, real duration) = 0;
};

class ParticleForceRegistry {
protected:
  struct ParticleForceRegistration {
    Particle *particle;
    ParticleForceGenerator *fg;
  };

  typedef std::vector<ParticleForceRegistration> Registry;
  Registry registrations;

public:
  void add(Particle *particle, ParticleForceGenerator *fg);
  void remove(Particle *particle, ParticleForceGenerator *fg);
  void clear();
  void updateForces(real duration);
};

class ParticleGravity : public ParticleForceGenerator {
public:
  ParticleGravity(const Vec3 &gravity);
  virtual void updateForce(Particle *particle, real duration);

private:
  Vec3 gravity;
};

class ParticleDrag : public ParticleForceGenerator {
private:
  real k1;
  real k2;

public:
  ParticleDrag(real k1, real k2);
  virtual void updateForce(Particle *particle, real duration);
};

class ParticleSpring : public ParticleForceGenerator {
private:
  Particle *other;
  real springConstant;
  real restLength;

public:
  ParticleSpring(Particle *other, real springConstant, real restLength);
  virtual void updateForce(Particle *particle, real duration);
};

class ParticleAnchoredSpring : public ParticleForceGenerator {
private:
  Vec3 *anchor;
  real springConstant;
  real restLength;

public:
  ParticleAnchoredSpring(Vec3 *anchor, real springConstant, real restLength);
  virtual void updateForce(Particle *particle, real duration);
};

class ParticleBungee : public ParticleForceGenerator {
private:
  Particle *other;
  real springConstant;
  real restLength;

public:
  ParticleBungee(Particle *other, real springConstant, real restLength);
  virtual void updateForce(Particle *particle, real duration);
};

class ParticleBuoyancy : public ParticleForceGenerator {
private:
  real maxDepth;
  real volume;
  real waterHeight;
  real liquidDensity;

public:
  ParticleBuoyancy(real maxDepth, real volume, real waterHeight,
                   real liquidDensity = 1000.0f);
  virtual void updateForce(Particle *particle, real duration);
};

} // namespace stingray
