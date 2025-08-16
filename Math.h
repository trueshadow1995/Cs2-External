#pragma once
#include "cmath"
#include "numbers"
#include "memory.h"


int screenwidth = 1920;
int screenheight = 1080;

struct View_Matrix_t {

	float* operator[](int index) {

     return Matrix[index];
    }
  float Matrix[4][4];
};


struct Vector3 {
  // constructor
  // struct data
  float x, y, z;
  constexpr Vector3(const float x = 0.f, const float y = 0.f,
                    const float z = 0.f) noexcept
      : x(x), y(y), z(z) {}

  // operator overloads
  constexpr const Vector3& operator-(const Vector3& other) const noexcept {
    return Vector3{x - other.x, y - other.y, z - other.z};
  }

  constexpr const Vector3& operator+(const Vector3& other) const noexcept {
    return Vector3{x + other.x, y + other.y, z + other.z};
  }

  constexpr const Vector3& operator/(const float factor) const noexcept {
    return Vector3{x / factor, y / factor, z / factor};
  }

  constexpr const Vector3& operator*(const float factor) const noexcept {
    return Vector3{x * factor, y * factor, z * factor};
  }

  // utils
  constexpr const Vector3& ToAngle() const noexcept {
    return Vector3{
        std::atan2(-z, std::hypot(x, y)) * (180.0f / std::numbers::pi_v<float>),
        std::atan2(y, x) * (180.0f / std::numbers::pi_v<float>), 0.0f};
  }

  constexpr const bool IsZero() const noexcept {
    return x == 0.f && y == 0.f && z == 0.f;
  }

 

  Vector3 WorldToScreen(View_Matrix_t matrix) const {
    float _x = matrix[0][0] * x + matrix[0][1] * y + matrix[0][2] * z
               +matrix[0][3];

    float _y = matrix[1][0] * x + matrix[1][1] * y + matrix[1][2] * z
               +matrix[1][3];

    float w = matrix[3][0] * x + matrix[3][1] * y + matrix[3][2] * z
              + matrix[3][3];


    if (w < 0.01f) 
        return false;

    float inv_w = 1.f / w; 
    _x *= inv_w;
    _y *= inv_w;

    float x = screenwidth * 0.5f;
    float y = screenheight * 0.5f;

    x += 0.5f * _x * screenwidth + 0.5f; 

    y -= 0.5f * _y * screenheight + 0.5f;

    return {x, y, w};

    

  }
  
};




struct vector2 {

    float x, y;
};








 
uintptr_t FindMeshOffset(uintptr_t pawn) {
  // Search through the first 0x500 bytes of the pawn object
  for (uintptr_t offset = 0; offset < 0x500; offset += 0x8) {
    uintptr_t candidate = mem.Read<uintptr_t>(pawn + offset);
    if (!candidate) continue;

    // Try to read a bone (example: bone[0] should exist and not be 0)
    Vector3 bonePos = mem.Read<Vector3>(candidate + 0x0);  // assume bone[0]
    if (bonePos.x != 0 && bonePos.y != 0 && fabs(bonePos.z) > 1.0f) {
      return offset;  // Found a likely mesh pointer

      
    }
  }
  return 0;
}



uintptr_t FindBoneArrayOffset(uintptr_t mesh) {
  for (int i = 0; i < 0x1000; i += 0x8) {
    uintptr_t candidate = mem.Read<uintptr_t>(mesh + i);
    if (!candidate) continue;

    // Read a test Vector3 from that memory
    Vector3 test = mem.Read<Vector3>(candidate);

    // Heuristic: a bone array usually has valid float coordinates
    if (fabs(test.x) > 0.01f && fabs(test.x) < 5000.f && fabs(test.y) > 0.01f &&
        fabs(test.y) < 5000.f && fabs(test.z) > 0.01f &&
        fabs(test.z) < 5000.f) {
      printf("[+] Bone array offset found: 0x%X\n", i);
      return i;
    }
  }
  return 0;
}




struct vec3 {
  float x, y, z;
};
struct Quaternion {
  float x, y, z, w;
};

struct CBoneData {
  vec3 location;
  float scale;
  Quaternion rotation;
};

struct BonePair {
  int bone1, bone2;
};

BonePair BoneConnections[] = {
    {0, 1},  {1, 2},   {2, 3},   {3, 4},  // spine
    {1, 5},  {5, 6},   {6, 7},            // left arm
    {1, 8},  {8, 9},   {9, 10},           // right arm
    {0, 11}, {11, 12}, {12, 13},          // left leg
    {0, 14}, {14, 15}, {15, 16},          // right leg
    {3, 17}, {17, 18}                     // neck → head
};

struct BoneLine {
  vec3 start;
  vec3 end;
};

CBoneData BoneArray[30];  // fixed array

// Read bones into BoneArray
void ReadBoneArray(uintptr_t boneArrayAddress) {
  for (int i = 0; i < 30; i++) {
    uintptr_t addr = boneArrayAddress + i * sizeof(CBoneData);
    BoneArray[i] = mem.Read<CBoneData>(addr);
  }
}

// Return an array of BoneLines instead of drawing
BoneLine* GetSkeletonLines(vec3 meshPos, int& outCount) {
  static BoneLine lines[32];  // max 32 connections
  outCount = 0;

  for (int i = 0; i < sizeof(BoneConnections) / sizeof(BoneConnections[0]);
       i++) {
    vec3 local1 = BoneArray[BoneConnections[i].bone1].location;
    vec3 local2 = BoneArray[BoneConnections[i].bone2].location;

    vec3 world1 = {local1.x + meshPos.x, local1.y + meshPos.y,
                   local1.z + meshPos.z};
    vec3 world2 = {local2.x + meshPos.x, local2.y + meshPos.y,
                   local2.z + meshPos.z};

    lines[outCount].start = world1;
    lines[outCount].end = world2;
    outCount++;
  }

  return lines;
}
