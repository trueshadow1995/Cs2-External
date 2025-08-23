#pragma once
#include "cmath"
#include "numbers"
#include "memory.h"

int screenwidth = 1920;

int screenheight = 1080;

struct ViewMatrix_t { //4x4 matrix
  float* operator[](int index) { return Matrix[index]; } // row access
  float Matrix[4][4]; 
}; 

struct Vector3 { //3d vector
  float x, y, z; //coordinates
  constexpr Vector3(const float x = 0.f, const float y = 0.f,
                    const float z = 0.f) noexcept // default values
      : x(x), y(y), z(z) {} //constructors

  constexpr Vector3 operator-(const Vector3& other) const noexcept {
    return Vector3{x - other.x, y - other.y, z - other.z}; //subtracts two vectors
  }

  constexpr Vector3 operator+(const Vector3& other) const noexcept {
    return Vector3{x + other.x, y + other.y, z + other.z}; //adds two vectors
  }

  constexpr Vector3 operator/(const float factor) const noexcept {
    return Vector3{x / factor, y / factor, z / factor}; //scales the fector by a factor
  }

  constexpr Vector3 operator*(const float factor) const noexcept { 
    return Vector3{x * factor, y * factor, z * factor}; //scales the vector by a factor
  }

  constexpr Vector3 ToAngle() const noexcept {
    return Vector3{
        std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
        std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>), 0.0f};    // returns angles in degrees
  }

  constexpr bool IsZero() const noexcept {
    return x == 0.f && y == 0.f && z == 0.f; 
  } //converts world coordinates to screen coordinates

  Vector3 WorldToScreen(ViewMatrix_t matrix) const { //4x4 matrix multiplication
    float _x =
        matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z + matrix[0][3]; //matrix row 0
    float _y =
        matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z + matrix[1][3]; //matrix row 1
    float w =
        matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z + matrix[3][3]; //matrix row 3

    if (w < 0.01f) return Vector3{0.f, 0.f, 0.f}; // behind the camera

    float inv_w = 1.f / w; //perspective division
    _x *= inv_w; //normalize x
    _y *= inv_w; //normalize y

    float sx = screenwidth * 0.5f; //half the screen width
    float sy = screenheight * 0.5f; //half the screen height

    sx += 0.5f * _x * screenwidth + 0.5f; //convert to screen coordinates
    sy -= 0.5f * _y * screenheight + 0.5f; // invert y axis

    return {sx, sy, w}; // return screen coordinates with depth value
  }
};

struct Quaternion {
  float x, y, z, w;
}; // quaternion for rotations

