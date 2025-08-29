#include "../Headers/BoneGlobal.h"

const BoneConnection BoneConnections[] = {
    // Head and neck
    {(int)BoneIDs::Head, (int)BoneIDs::Neck},
    {(int)BoneIDs::Neck, (int)BoneIDs::UpperChest},

    // Torso
    {(int)BoneIDs::UpperChest, (int)BoneIDs::LowerChest},
    {(int)BoneIDs::LowerChest, (int)BoneIDs::Stomach},
    {(int)BoneIDs::Stomach, (int)BoneIDs::Pelvis},

    // Left arm
    {(int)BoneIDs::Neck, (int)BoneIDs::LeftShoulder},
    {(int)BoneIDs::LeftShoulder, (int)BoneIDs::LeftElbow},
    {(int)BoneIDs::LeftElbow, (int)BoneIDs::LeftArm},

    // Right arm
    {(int)BoneIDs::Neck, (int)BoneIDs::RightShoulder},
    {(int)BoneIDs::RightShoulder, (int)BoneIDs::RightElbow},
    {(int)BoneIDs::RightElbow, (int)BoneIDs::RightArm},

    // Left leg
    {(int)BoneIDs::Pelvis, (int)BoneIDs::LeftThigh},
    {(int)BoneIDs::LeftThigh, (int)BoneIDs::LeftKnee},
    {(int)BoneIDs::LeftKnee, (int)BoneIDs::LeftLeg},
    {(int)BoneIDs::LeftLeg, (int)BoneIDs::LeftFoot},

    // Right leg
    {(int)BoneIDs::Pelvis, (int)BoneIDs::RightThigh},
    {(int)BoneIDs::RightThigh, (int)BoneIDs::RightKnee},
    {(int)BoneIDs::RightKnee, (int)BoneIDs::RightLeg},
    {(int)BoneIDs::RightLeg, (int)BoneIDs::RightFoot}};

const size_t BoneConnectionsCount =
    sizeof(BoneConnections) / sizeof(BoneConnection);
