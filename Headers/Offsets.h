#pragma once

namespace offsets {
constexpr std::ptrdiff_t EntityList = 0x1D154C8;
constexpr std::ptrdiff_t LocalPlayerPawn = 0x1BF14A0;
constexpr std::ptrdiff_t ViewMatrix = 0x1E32830;
 constexpr std::ptrdiff_t dwViewAngles = 0x1E3D5C0;


 constexpr std::ptrdiff_t m_iMaxHealth = 0x348;
 constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;  
 constexpr std::ptrdiff_t m_hPawn =0x6B4;  // CHandle<C_BasePlayerPawn :o - lol phil >
 constexpr std::ptrdiff_t m_vOldOrigin = 0x15B0;  // Vector
 constexpr std::ptrdiff_t m_iHealth = 0x34C;      // int32
 constexpr std::ptrdiff_t Mesh = 0x38;

  constexpr std::ptrdiff_t dwLocalPlayerController = 0x1E1E798;

 constexpr std::ptrdiff_t m_pGameSceneNode = 0x330;  // CGameSceneNode*


   
 
   constexpr std::ptrdiff_t m_modelState = 0x190;
   
   constexpr std::ptrdiff_t boneArrayOffset = 0x80;  // inside modelState


}
