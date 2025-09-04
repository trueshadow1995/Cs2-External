#pragma once
#include <cstdint>

namespace globals {
inline std::uintptr_t clientAddress = 0;
inline std::uintptr_t engine = 0;
constexpr int MAX_BONES = 60;

inline float FovColor[3]{0.f, 0.f, 1.f};

inline bool Chams = false;
inline bool Rainbowchams = false;
inline float chamsbrightness = 25.f;
inline float EnemyChamColor[]{1.f, 0.f, 0.f, 4.f};
inline float TeamChamColor[]{0.f, 1.f, 0.f, 4.f};
inline float MyChamColor[]{1.0f, 1.0f, 1.0f};
inline bool skinchanger = false;
inline float MouseSensitivity = 1.0f;  // Match CS2 in-game sensitivity (e.g., 1.0f)
inline bool AimbotSmoothing = false;  // Toggle for smoothing
inline float Aimbotsmoothing = 0.5f;  // Smoothing strength (0.01 to 1.0)
inline bool Aimbot = false;
inline float AimbotFovSize = 10.f;
inline float AimbotMaxDistance = 0.0f;
inline int AimbotBone = 0;
inline bool AimBotAutofire = true;

inline bool TeammateNameEsp = true;
inline bool EnemyNameEsp = true;
inline bool TeammateDistanceEsp = true;
inline bool EnemyDistanceEsp = true;
inline bool NameEsp = true;
inline bool DistanceEsp = true;

inline float PlayerNameColor[3] = {1.f, 0.f, 0.f};

inline bool WaterMark = false;
inline float SnapLineColor[3] = {0.f, 1.f, 0.f};
//esp box color ?? 
inline float EspBoxColor[3] = {0.f, 1.f, 0.f};
//player esp background ?? wtf is that even doing there
inline bool PlayerEspBackGround = false;
//fps counter
inline bool FpsCounter = false;
//Head marker stuff
inline bool Headmarker = false;
inline float headMakerColor[3] = {0.f, 1.f, 0.f};
inline float Headmarkersize = 3.f;

// health bars
inline bool HealthBar =false;
inline bool TeammateHealth = false;
inline bool TeammateHealthTxt = false;
inline float TeammateHealthColor[4] = {0.f, 255.f, 0.f, 255.f};  // green
inline bool EnemyHealth = false;
inline bool EnemyHealthText = false;
inline float EnemyHealthColor[4] = {0.f, 255.f, 0.f, 255.f};  // red
inline bool HealthPercentage = false; 

//bone colors
inline bool Bones = false;
inline float TeammateBoneColor[4] = {0.f, 255.f, 0.f, 255.f};  // green
inline float EnemyBoneColor[4] = {255.f, 0.f, 0.f, 255.f};     // red
inline bool EnemyBones = false;
inline bool FriendlyBones = true;
inline float BoneEspThickness = 2.0f;

//menu toogle
inline bool menu_open = false;
//esp stuff
inline bool Esp = false;
inline bool EnemyEsp = false;
inline bool TeammateEsp = false;
inline bool EnemyEspBackground = false;
inline bool TeammateEspBackground = false;
inline float EnemyEspColor[4] = {1.f, 0.f, 0.f, 1.f};     // red
inline float TeammateEspColor[4] = {0.f, 1.f, 0.f, 1.f};  // green
inline float EnemyEspBackGroundColor[4] = {1.f, 0.f, 0.f, 150};
inline float FriendlyEspBackGroundColor[4] = {0.f, 1.f, 0.f, };
//bone debug stolen from Uc :D 
inline bool BoneDebug = false;

}  // namespace globals
