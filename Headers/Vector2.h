#pragma once
#include "Math.h"

struct Vector2 {
  float x, y;

  constexpr Vector2(float x_ = 0.f, float y_ = 0.f) noexcept : x(x_), y(y_) {}

  constexpr Vector2 operator+(const Vector2& other) const noexcept {
    return Vector2{x + other.x, y + other.y};
  }

  constexpr Vector2 operator-(const Vector2& other) const noexcept {
    return Vector2{x - other.x, y - other.y};
  }

  constexpr bool IsOnScreen(float screenWidth,
                            float screenHeight) const noexcept {
    return x >= 0.f && x <= screenWidth && y >= 0.f && y <= screenHeight;
  }

  constexpr bool IsValid() const noexcept { return x >= 0.f && y >= 0.f; }
};


