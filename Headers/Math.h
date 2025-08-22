#pragma once
#include "cmath"
#include "numbers"
#include "memory.h"

int screenwidth = 1920;

int screenheight = 1080;

struct ViewMatrix_t {
  float* operator[](int index) { return Matrix[index]; }
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

  Vector3 WorldToScreen(ViewMatrix_t matrix) const {
    float _x =
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3];
    float _y =
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3];
    float w =
        matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3];

    if (w < 0.01f) return Vector3{0.f, 0.f, 0.f};

    float inv_w = 1.f / w;
    _x *= inv_w;
    _y *= inv_w;

    float sx = screenwidth * 0.5f;
    float sy = screenheight * 0.5f;

    sx += 0.5f * _x * screenwidth + 0.5f;
    sy -= 0.5f * _y * screenheight + 0.5f;

    return {sx, sy, w};
  }
};

struct Quaternion {
  float x, y, z, w;
};

