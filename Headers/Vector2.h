#pragma once
#include "Math.h"

struct Vector2 { // 2d vector
  float x, y;

  constexpr Vector2(float x_ = 0.f, float y_ = 0.f) noexcept : x(x_), y(y_) {} // constructor with default values

  constexpr Vector2 operator+(const Vector2& other) const noexcept { // addition operator
    return Vector2{x + other.x, y + other.y}; // add two vectors
  }

  constexpr Vector2 operator-(const Vector2& other) const noexcept { // subtraction operator
    return Vector2{x - other.x, y - other.y}; // subtract two vectors
  }

  constexpr bool IsOnScreen(float screenWidth,
                            float screenHeight) const noexcept { 
    return x >= 0.f && x <= screenWidth && y >= 0.f && y <= screenHeight; // check if vector is on screen
  }

  constexpr bool IsValid() const noexcept { return x >= 0.f && y >= 0.f; } // check if vector is valid
};


