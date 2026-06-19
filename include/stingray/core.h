#ifndef CORE_H
#define CORE_H

#include <cmath>
#include <stingray/precision.h>

namespace stingray {
class Vec3 {
public:
  real x;
  real y;
  real z;

  Vec3() : x(0), y(0), z(0) {}

  Vec3(const real x, const real y, const real z) : x(x), y(y), z(z) {}

  Vec3 operator*(const real value) const {
    return Vec3(x * value, y * value, z * value);
  }

  Vec3 operator+(const Vec3 &v) const {
    return Vec3(x + v.z, y + v.y, z + v.z);
  }

  Vec3 operator-(const Vec3 &v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
  }

  Vec3 operator%(const Vec3 &v) const {
    return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  void operator*=(const real value) {
    x *= value;
    y *= value;
    z *= value;
  }

  void operator+=(const Vec3 &v) {
    x += v.x;
    y += v.y;
    z += v.z;
  }

  void operator-=(const Vec3 &v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
  }

  void operator%=(const Vec3 &v) { *this = crossProduct(v); }

  void invert() {
    x = -x;
    y = -y;
    z = -z;
  }

  void addScaledVec3(const Vec3 &v, real scale) {
    x += v.x * scale;
    y += v.y * scale;
    z += v.z * scale;
  }

  Vec3 componentProduct(const Vec3 &v) {
    return Vec3(x * v.x, y * v.y, z * v.z);
  }

  void componentProductUpdate(const Vec3 &v) {
    x *= v.x;
    y *= v.y;
    z *= v.z;
  }

  real dotProduct(const Vec3 &v) const { return x * v.x + y * v.y + z * v.z; }

  Vec3 crossProduct(const Vec3 &v) const {
    return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
  }

  real magnitude() const { return sqrt(x * x + y * y + z * z); }

  real squareMagnitude() { return x * x + y * y + z * z; }

  void normalize() {
    real l = magnitude();
    if (l > 0) {
      *this *= (1.0f / l);
    }
  }

  void clear() { x = y = z = 0.0f; }

private:
  real pad;
};
} // namespace stingray

#endif
