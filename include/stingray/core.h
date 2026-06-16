#include "precision.h"
namespace stingray {
class Vec3 {
public:
  real x;
  real y;
  real z;

  Vec3() : x(0), y(0), z(0) {}
  Vec3(const real x, const real y, const real z) : x(x), y(y), z(z) {}
  void invert() {
    x = -x;
    y = -y;
    z = -z;
  }
  real magnitude() const { return sqrt(x * x + y * y + z * z); }
  real squareMagnitude() { return x * x + y * y + z * z; }
  void normalize() {
    real l = magnitude();
    if (l > 0) {
      *this *= (1.0f / l);
    }
  }

private:
  real pad;
};
} // namespace stingray
