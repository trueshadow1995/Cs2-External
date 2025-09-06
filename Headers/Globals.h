#pragma once
#include <cstdint>
#ifndef VK_XBUTTON1
#define VK_XBUTTON1 0x05
#define VK_XBUTTON2 0x06
#endif

namespace globals {
inline std::uintptr_t clientAddress = 0;
inline std::uintptr_t engine = 0;
constexpr int MAX_BONES = 60;

//random not working yet
inline bool Chams = false;
inline bool Rainbowchams = false;
inline float chamsbrightness = 25.f;
inline float EnemyChamColor[4]{1.f, 0.f, 0.f, 4.f};
inline float TeamChamColor[4]{0.f, 1.f, 0.f, 4.f};
inline float MyChamColor[3]{1.0f, 1.0f, 1.0f};
inline bool skinchanger = false;
//aimbot stuff

inline int AimbotKey = VK_XBUTTON1;  // Default to mouse side button
inline float MouseSensitivity = 1.0f;
inline bool AimbotUseSmoothing = false;  // toggle
inline float AimbotSmoothAmount = 0.5f;  // 0.01f ? 1.0f
inline bool Aimbot = false;
inline float AimbotFovSize = 10.f;
inline int FovStyle = 0;
inline float AimbotMaxDistance = 0.0f;
inline int AimbotBone = 0;
inline bool AimBotAutofire = false;
inline float FovColor[3]{0.f, 0.f, 1.f};
inline bool CrossHair = false;
 inline bool FovCircle = false;
// ESP stuff
inline bool TeammateNameEsp = false;
inline bool EnemyNameEsp = false;
inline bool TeammateDistanceEsp = false;
inline bool EnemyDistanceEsp = false;
inline bool NameEsp = false;
inline bool DistanceEsp = false;

inline float PlayerNameColor[4] = {1.f, 0.f, 0.f, 1.f};
inline float PlayerDistanceColor[4] = {1.f, 1.f, 1.f, 1.f};  // White by default
inline bool CornerEspBoxStyle = false;
inline bool WaterMark = false;
inline float SnapLineColor[3] = {0.f, 1.f, 0.f};

// ESP box color
inline float EspBoxColor[4] = {0.f, 1.f, 0.f, 1.f};

// Player ESP background
inline bool PlayerEspBackGround = false;

// FPS counter
inline bool FpsCounter = false;

// Head marker stuff
inline bool Headmarker = false;
inline float headMakerColor[4] = {0.f, 1.f, 0.f, 1.f};
inline float Headmarkersize = 3.f;

// Health bars
inline bool HealthBar = false;
inline bool TeammateHealth = false;
inline bool TeammateHealthTxt = false;
inline float TeammateHealthColor[4] = {0.f, 1.f, 0.f, 1.f};  // green
inline bool EnemyHealth = false;
inline bool EnemyHealthText = false;
inline float EnemyHealthColor[4] = {1.f, 0.f, 0.f, 1.f};  // red
inline bool HealthPercentage = false;

// Bone colors
inline bool Bones = false;
inline float TeammateBoneColor[4] = {0.f, 1.f, 0.f, 1.f};  // green
inline float EnemyBoneColor[4] = {1.f, 0.f, 0.f, 1.f};     // red
inline bool EnemyBones = false;
inline bool FriendlyBones = false;
inline float BoneEspThickness = 2.0f;

// Menu toggle
inline bool menu_open = false;

// ESP stuff
inline bool Esp = false;
inline bool EnemyEsp = false;
inline bool TeammateEsp = false;
inline bool EnemyEspBackground = false;
inline bool TeammateEspBackground = false;
inline float EnemyEspColor[4] = {1.f, 0.f, 0.f, 1.f};     // red
inline float TeammateEspColor[4] = {0.f, 1.f, 0.f, 1.f};  // green
inline float EnemyEspBackGroundColor[4] = {1.f, 0.f, 0.f, 0.6f};
inline float FriendlyEspBackGroundColor[4] = {0.f, 1.f, 0.f, 0.6f};

// Bone debug
inline bool BoneDebug = false;

// Player health color (for health-based coloring)
inline float PlayerHealthColor[4] = {0.f, 1.f, 0.f, 1.f};

}  // namespace globals