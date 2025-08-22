#pragma once
#include "../Headers/Math.h"
#include <array>




struct CBoneData {
  Vector3 location;                
  char pad[0x20 - sizeof(Vector3)]; 
};


static_assert(sizeof(CBoneData) == 0x20);


CBoneData BoneArray[128];


struct CUtlVector {
  uintptr_t data;  
  int count;
  int capacity;
};


struct BoneArrayInfo {
  uintptr_t data;
  int count;
};


struct BonePair {
  int bone1, bone2;
};

enum class BoneIDs {
  Head = 6,
  Neck = 5,
  UpperChest = 4,
  LowerChest = 3,
  Stomach = 2,
  Pelivs = 0,

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
  LeftPinky2 = 42,
  LeftPinky3 = 43,
  LeftPinky4 = 44,

  LeftRing1 = 52,
  LeftRing2 = 53,
  LeftRing3 = 54,
  LeftRing4 = 55,

  LeftMiddle1 = 37,
  LeftMiddle2 = 38,
  LeftMiddle3 = 39,
  LeftMiddle4 = 40,

  LeftIndex1 = 45,
  LeftIndex2 = 46,
  LeftIndex3 = 47,
  LeftIndex4 = 48,

  LeftThumb1 = 49,
  LeftThumb2 = 50,
  LeftThumb3 = 51,

  RightPinky1 = 66,
  RightPinky2 = 67,
  RightPinky3 = 68,
  RightPinky4 = 69,

  RightRing1 = 77,
  RightRing2 = 78,
  RightRing3 = 79,
  RightRing4 = 80,

  RightMiddle1 = 62,
  RightMiddle2 = 63,
  RightMiddle3 = 64,
  RightMiddle4 = 65,

  RightIndex1 = 70,
  RightIndex2 = 71,
  RightIndex3 = 72,
  RightIndex4 = 73,

  RightThumb1 = 74,
  RightThumb2 = 75,
  RightThumb3 = 76,
};

BonePair BoneConnections[] = {
    // Spine
    {(int)BoneIDs::Pelivs, (int)BoneIDs::Stomach},
    {(int)BoneIDs::Stomach, (int)BoneIDs::LowerChest},
    {(int)BoneIDs::LowerChest, (int)BoneIDs::UpperChest},
    {(int)BoneIDs::UpperChest, (int)BoneIDs::Neck},

    // Left arm
    {(int)BoneIDs::Neck, (int)BoneIDs::LeftShoulder},
    {(int)BoneIDs::LeftShoulder, (int)BoneIDs::LeftElbow},
    {(int)BoneIDs::LeftElbow, (int)BoneIDs::LeftArm},

    // Right arm
    {(int)BoneIDs::Neck, (int)BoneIDs::RightShoulder},
    {(int)BoneIDs::RightShoulder, (int)BoneIDs::RightElbow},
    {(int)BoneIDs::RightElbow, (int)BoneIDs::RightArm},

    // Left leg
    {(int)BoneIDs::Pelivs, (int)BoneIDs::LeftThigh},
    {(int)BoneIDs::LeftThigh, (int)BoneIDs::LeftKnee},
    {(int)BoneIDs::LeftKnee, (int)BoneIDs::LeftLeg},
    {(int)BoneIDs::LeftLeg, (int)BoneIDs::LeftFoot},

    // Right leg
    {(int)BoneIDs::Pelivs, (int)BoneIDs::RightThigh},
    {(int)BoneIDs::RightThigh, (int)BoneIDs::RightKnee},
    {(int)BoneIDs::RightKnee, (int)BoneIDs::RightLeg},
    {(int)BoneIDs::RightLeg, (int)BoneIDs::RightFoot},

    // Neck -> Head
    {(int)BoneIDs::Neck, (int)BoneIDs::Head}};

std::array<CBoneData, 30> ReadBoneArray(uintptr_t currentEntity,
                                        uintptr_t m_pGameSceneNode,
                                        uintptr_t m_modelState) {
  uintptr_t gameSceneNode =
      mem.Read<uintptr_t>(currentEntity + m_pGameSceneNode);
  uintptr_t boneArrayPtr =
      mem.Read<uintptr_t>(gameSceneNode + m_modelState + 0x80);
  return mem.Read<std::array<CBoneData, 30>>(boneArrayPtr);
}