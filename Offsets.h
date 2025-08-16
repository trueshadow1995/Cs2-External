#pragma once

namespace offsets {
 constexpr std::ptrdiff_t EntityList = 0x1D15578;
 constexpr std::ptrdiff_t LocalPlayerPawn = 0x1BF14A0;
 constexpr std::ptrdiff_t ViewMatrix = 0x1E328A0;
 constexpr std::ptrdiff_t dwViewAngles = 0x1E3D780;


 constexpr std::ptrdiff_t m_iMaxHealth = 0x348;
 constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;  
 constexpr std::ptrdiff_t m_hPawn =0x6B4;  // CHandle<C_BasePlayerPawn :o - lol phil >
 constexpr std::ptrdiff_t m_vOldOrigin = 0x15B0;  // Vector
 constexpr std::ptrdiff_t m_iHealth = 0x34C;      // int32
}
