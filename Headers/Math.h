#pragma once
#include "Math.h"
#include "cmath"
#include "memory.h"
#include "numbers"
#include <algorithm>

static int screenheight = 1080;
static int screenwidth = 1920;

struct ViewMatrix_t {
  float* operator[](int index) { return Matrix[index]; }
  const float* operator[](int index) const { return Matrix[index]; }
  float Matrix[4][4];
};

struct Vector3 {
  float x, y, z;

  constexpr Vector3(const float x = 0.f, const float y = 0.f,
                    const float z = 0.f) noexcept
      : x(x), y(y), z(z) {}

  constexpr Vector3 operator-(const Vector3& other) const noexcept {
    return Vector3{x - other.x, y - other.y, z - other.z};
  }

  constexpr Vector3 operator+(const Vector3& other) const noexcept {
    return Vector3{x + other.x, y + other.y, z + other.z};
  }

  constexpr Vector3 operator/(const float factor) const noexcept {
    return Vector3{x / factor, y / factor, z / factor};
  }

  constexpr Vector3 operator*(const float factor) const noexcept {
    return Vector3{x * factor, y * factor, z * factor};
  }

  constexpr Vector3 ToAngle() const noexcept {
    return Vector3{
        std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
        std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>), 0.0f};
  }

  constexpr bool IsZero() const noexcept {
    return x == 0.f && y == 0.f && z == 0.f;
  }

  // ADDED: Distance calculation method
  float Distance(const Vector3& other) const noexcept {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
  }

  bool WorldToScreen(const ViewMatrix_t& matrix, float screenWidth,
                     float screenHeight, Vector3& out) const {
    float clipX =
        x * matrix[0][0] + y * matrix[0][1] + z * matrix[0][2] + matrix[0][3];
    float clipY =
        x * matrix[1][0] + y * matrix[1][1] + z * matrix[1][2] + matrix[1][3];
    float clipZ =
        x * matrix[2][0] + y * matrix[2][1] + z * matrix[2][2] + matrix[2][3];
    float clipW =
        x * matrix[3][0] + y * matrix[3][1] + z * matrix[3][2] + matrix[3][3];

    if (clipW < 0.01f) return false;  // behind camera

    float invW = 1.f / clipW;
    out.x = (screenWidth / 2.f) * (clipX * invW + 1.f);
    out.y = (screenHeight / 2.f) * (1.f - clipY * invW);  // flip Y
    out.z = clipZ * invW;

    return true;
  }


  // In your Math.h, update the Vector3::Lerp function:
  static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
    // Manual clamping instead of std::clamp
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t};
  }
    // Smooth interpolation (ease-in-out)
    static Vector3 SmoothLerp(const Vector3& a, const Vector3& b, float t) {
      t = std::clamp(t, 0.0f, 1.0f);
      // Smooth step function
      t = t * t * (3.0f - 2.0f * t);
      return Lerp(a, b, t);
    }
  
};


struct Quaternion {
  float x, y, z, w;

  Quaternion() : x(0), y(0), z(0), w(1) {}
  Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

