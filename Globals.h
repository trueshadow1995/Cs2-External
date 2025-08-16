#pragma once

namespace globals {
inline std::uintptr_t clientAddress = 0;
inline std::uintptr_t engine = 0;
inline bool playerhealth = true;
inline float EnemyGlowColor[]{1.f, 0.f, 0.f};
inline bool playerespfullbox = false;
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
inline bool watamark = false;
inline float AimbotFovSize = 3.f;
inline bool PlayerName = false;
inline bool PlayerHealth = false;
inline bool SnapLines = false;
inline float PlayerNameColor[3] = {1.f, 0.f, 0.f};
inline float PlayerHealthColor[3] = {0.f, 1.f, 0.f};
inline float SnapLineColor[3] = {0.f, 1.f, 0.f};
inline float EspBoxColor[3] = {0.f, 1.f, 0.f};
inline bool PlayerEspBackGround = false;
inline bool FpsCounter = false;
inline bool Headmarker = false;
inline float headMakerColor[3] = {0.f, 1.f, 0.f};
inline float Headmarkersize = 3.f;

}  // namespace globals
