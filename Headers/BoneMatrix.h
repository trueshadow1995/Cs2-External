#pragma once
#include "Math.h"

struct CBoneData {
  Vector3 location;  // Bone position in world coordinates

  CBoneData() : location(0, 0, 0) {}
  char pad[0x20 - sizeof(Vector3)];
  bool IsZero() const {
    return location.x == 0 && location.y == 0 && location.z == 0;
  }
};

struct Matrix3x4_t {
  float m[3][4];
  Vector3 GetOrigin() const { return {m[0][3], m[1][3], m[2][3]}; }
};