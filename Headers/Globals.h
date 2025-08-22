#pragma once
#include <cstdint>

namespace globals {
inline std::uintptr_t clientAddress = 0;
inline std::uintptr_t engine = 0;
inline bool playerhealth = true;

inline bool playerespcorner = false;
inline bool radar = false;
inline bool Bhop = false;
inline bool Triggerbot = false;
inline float Fovamount = 90.f;
inline bool Fov = false;
inline bool NoFlash = false;
inline bool Aimbot = false;
inline bool fovchanger = false;
inline bool NoRecoil = false;
inline float FovColor[3]{0.f, 0.f, 1.f};
inline bool Smoothing = false;
inline bool Chams = false;
inline bool Rainbowchams = false;
inline float chamsbrightness = 25.f;
inline float EnemyChamColor[]{1.f, 0.f, 0.f, 4.f};
inline float TeamChamColor[]{0.f, 1.f, 0.f, 4.f};
inline float MyChamColor[]{1.0f, 1.0f, 1.0f};
inline bool skinchanger = false;
inline bool AimbotSmoothing = false;
inline float Aimbotsmoothing = 5.f;
inline bool WaterMark = false;
inline float AimbotFovSize = 3.f;
inline bool PlayerName = false;
inline bool SnapLines = false;
inline float PlayerNameColor[3] = {1.f, 0.f, 0.f};
inline float SnapLineColor[3] = {0.f, 1.f, 0.f};
inline float EspBoxColor[3] = {0.f, 1.f, 0.f};

inline bool PlayerEspBackGround = false;


inline bool FpsCounter = false;

inline bool Headmarker = false;
inline float headMakerColor[3] = {0.f, 1.f, 0.f};
inline float Headmarkersize = 3.f;



inline bool TeammateHealth = false;
inline float TeammateHealthColor[4] = {0.f, 255.f, 0.f, 255.f};  // greenish

inline bool EnemyHealth = false;
inline float EnemyHealthColor[4] = {255.f, 0.f, 0.f, 255.f};  // red


inline float TeammateBoneColor[4] = {0.f, 255.f, 0.f, 255.f}; // green
inline float EnemyBoneColor[4] = {255.f, 0.f, 0.f, 255.f};     // red




inline bool EnemyBones = true;
inline bool FriendlyBones = false;
inline float BoneEspThickness = 2.0f;                 

extern bool menu_open;



inline bool EnemyEsp = false;
inline bool TeammateEsp = false;

inline bool EnemyEspBackground = false;
inline bool TeammateEspBackground = false;

inline float EnemyEspColor[4] = {1.f, 0.f, 0.f, 1.f};     // red
inline float TeammateEspColor[4] = {0.f, 1.f, 0.f, 1.f};  // green
inline float EnemyEspBackGroundColor[4] = {1.f, 0.f, 0.f, 60};
inline float FriendlyEspBackGroundColor[4] = {0.f, 1.f, 0.f, 60};

inline bool BoneDebug = false;






}  // namespace globals
