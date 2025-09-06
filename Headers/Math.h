#pragma once
#include <algorithm>
#include <cmath>
#include <numbers>

static int screenheight = 1080;
static int screenwidth = 1920;

struct ViewMatrix_t {
  float* operator[](int index) { return Matrix[index]; }
  const float* operator[](int index) const { return Matrix[index]; }
  float Matrix[4][4];
};

struct Vector3 {
  float x, y, z;

  constexpr Vector3(float x = 0.f, float y = 0.f, float z = 0.f) noexcept
      : x(x), y(y), z(z) {}

  constexpr Vector3 operator-(const Vector3& other) const noexcept {
    return Vector3{x - other.x, y - other.y, z - other.z};
  }

  constexpr Vector3 operator+(const Vector3& other) const noexcept {
    return Vector3{x + other.x, y + other.y, z + other.z};
  }

  constexpr Vector3 operator/(float factor) const noexcept {
    return Vector3{x / factor, y / factor, z / factor};
  }

  constexpr Vector3 operator*(float factor) const noexcept {
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

  float Distance(const Vector3& other) const {
    float dx = x - other.x;
    float dy = y - other.y;
    float dz = z - other.z;
    float distSq = dx * dx + dy * dy + dz * dz;

    // Prevent sqrt of negative or very small numbers
    if (distSq < 0.0001f) return 0.0f;
    return std::sqrt(distSq);
  }

  // FIXED WorldToScreen function
  bool WorldToScreen(const ViewMatrix_t& matrix, float screenWidth,
                     float screenHeight, Vector3& out) const {
    if (screenWidth <= 0 || screenHeight <= 0) return false;

    // Get the view matrix
    const float* m = &matrix.Matrix[0][0];

    // Transform world coordinates to clip space
    float clipX = x * m[0] + y * m[1] + z * m[2] + m[3];
    float clipY = x * m[4] + y * m[5] + z * m[6] + m[7];
    float clipZ = x * m[8] + y * m[9] + z * m[10] + m[11];
    float clipW = x * m[12] + y * m[13] + z * m[14] + m[15];

    // Check if point is behind the camera
    if (clipW < 0.1f) return false;

    // Perspective division
    float invW = 1.0f / clipW;
    float ndcX = clipX * invW;
    float ndcY = clipY * invW;

    // Convert to screen coordinates - KEEP ESP CORRECT
    out.x = (screenWidth / 2.0f) * ndcX + (screenWidth / 2.0f);
    out.y = (screenHeight / 2.0f) -
            (screenHeight / 2.0f) * ndcY;  // Keep minus for ESP
    out.z = clipZ * invW;

    // Check if point is within screen bounds
    return out.x >= 0 && out.x <= screenWidth && out.y >= 0 &&
           out.y <= screenHeight && out.z >= 0;
  }

  static Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    return {a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
            a.z + (b.z - a.z) * t};
  }

  static Vector3 SmoothLerp(const Vector3& a, const Vector3& b, float t) {
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;

    t = t * t * (3.0f - 2.0f * t);
    return Lerp(a, b, t);
  }

  // Simple validation to prevent NaN issues
  bool IsValid() const {
    return !std::isnan(x) && !std::isnan(y) && !std::isnan(z);
  }
};

struct Quaternion {
  float x, y, z, w;

  Quaternion() : x(0), y(0), z(0), w(1) {}
  Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
};

// Simple utility function for screen bounds checking
inline bool IsOnScreen(float x, float y, float screenWidth,
                       float screenHeight) {
  return x >= 0 && x <= screenWidth && y >= 0 && y <= screenHeight;
}