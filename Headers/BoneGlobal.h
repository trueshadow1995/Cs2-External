#pragma once
#include <cstdint>

// Define your BoneConnection struct and enum
struct BoneConnection {
  int bone1, bone2;
};

enum class BoneIDs {
  Head = 6,
  Neck = 5,
  UpperChest = 4,
  LowerChest = 3,
  Stomach = 2,
  Pelvis = 0,
  LeftShoulder = 8,
  LeftElbow = 9,
  LeftArm = 10,
  RightShoulder = 13,
  RightElbow = 14,
  RightArm = 15,
  LeftThigh = 22,
  LeftKnee = 23,
  LeftLeg = 30,
  LeftFoot = 98,
  RightThigh = 25,
  RightKnee = 26,
  RightLeg = 32,
  RightFoot = 101,
  LeftPinky1 = 41,
  LeftRing1 = 52,
  LeftMiddle1 = 37,
  LeftIndex1 = 45,
  LeftThumb1 = 49,
  RightPinky1 = 66,
  RightRing1 = 77,
  RightMiddle1 = 62,
  RightIndex1 = 70,
  RightThumb1 = 74,
};

// Declare your bone connections array
extern const BoneConnection BoneConnections[];
extern const size_t BoneConnectionsCount;