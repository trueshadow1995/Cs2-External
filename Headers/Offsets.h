#pragma once
#include <cstddef>
namespace offsets {
// offsets.hpp
constexpr std::ptrdiff_t dwCSGOInput = 0x1E287F0;
constexpr std::ptrdiff_t dwEntityList = 0x1D00690;
constexpr std::ptrdiff_t dwGameEntitySystem = 0x1FA4FD0;
constexpr std::ptrdiff_t dwGameEntitySystem_highestEntityIndex = 0x20F0;
constexpr std::ptrdiff_t dwGameRules = 0x1E1DB80;
constexpr std::ptrdiff_t dwGlobalVars = 0x1BD0E50;
constexpr std::ptrdiff_t dwGlowManager = 0x1E1AA18;
constexpr std::ptrdiff_t dwLocalPlayerController = 0x1E0A348;
constexpr std::ptrdiff_t dwLocalPlayerPawn = 0x1BDBB10;
constexpr std::ptrdiff_t dwPlantedC4 = 0x1E23278;
constexpr std::ptrdiff_t dwPrediction = 0x1BDBA40;
constexpr std::ptrdiff_t dwSensitivity = 0x1E1B478;
constexpr std::ptrdiff_t dwSensitivity_sensitivity = 0x50;
constexpr std::ptrdiff_t ViewAngles = 0x1E28EA0;
constexpr std::ptrdiff_t ViewMatrix = 0x1E1E920;
constexpr std::ptrdiff_t dwViewRender = 0x1E1F760;
constexpr std::ptrdiff_t dwWeaponC4 = 0x1DBB700;
// client.hpp
constexpr std::ptrdiff_t m_iTeamNum = 0x3EB;  // uint8
constexpr std::ptrdiff_t m_hPawn = 0x6B4;  // CHandle<C_BasePlayerPawn :o // lol phil >
constexpr std::ptrdiff_t m_hPlayerPawn = 0x8FC;     // CHandle<C_CSPlayerPawn>
constexpr std::ptrdiff_t m_iHealth = 0x34C;         // int32
constexpr std::ptrdiff_t Mesh = 0x38;               //  -> aids to find
constexpr std::ptrdiff_t EngineClient = 0x17E6C50;  // IEngineClient interface
constexpr std::ptrdiff_t m_modelState = 0x190;      // CModelState
constexpr std::ptrdiff_t boneArrayOffset =  0x80;  // inside modelState <- also aids to get
constexpr std::ptrdiff_t m_vecOrigin = 0x88;              // Vector
constexpr std::ptrdiff_t m_pGameSceneNode = 0x330;        // CGameSceneNode*
constexpr std::ptrdiff_t BoneMatrix = 0xEC8;              // Verified from debug
constexpr std::ptrdiff_t m_CBodyComponent = 0x38;         // CBodyComponent*
constexpr std::ptrdiff_t m_groundNormal = 0x258;          // Vector
constexpr std::ptrdiff_t m_sSanitizedPlayerName = 0x850;  // CUtlString
constexpr std::ptrdiff_t m_bDormant = 0x10B;              // bool
constexpr std::ptrdiff_t m_vOldOrigin = 0x15A0;
constexpr uintptr_t m_vecAbsOrigin = 0xD0;         // VectorWS
constexpr std::ptrdiff_t m_iszPlayerName = 0x6E8;  // char[128]
constexpr std::ptrdiff_t m_hMyWeapons = 0x40;
constexpr uint32_t m_pClippingWeapon = 0x3DE0;        // C_CSWeaponBase*
constexpr std::ptrdiff_t m_pWeaponServices = 0x13F0;  // CPlayer_WeaponServices*
constexpr uint32_t m_hActiveWeapon = 0x58;       // CHandle<C_BasePlayerWeapon>
constexpr uint32_t m_AttributeManager = 0x1390;  // C_AttributeContainer
constexpr uint32_t m_iItemDefinitionIndex = 0x1BA;  // uint16
constexpr std::ptrdiff_t m_Item = 0x50;             // C_EconItemView
constexpr std::ptrdiff_t m_fFlags = 0x3F8;          // uint32
constexpr std::ptrdiff_t m_vecVelocity = 0x430;     // CNetworkVelocityVector
constexpr std::ptrdiff_t m_MoveType = 0x525;        // MoveType_t
constexpr std::ptrdiff_t m_angEyeAngles = 0x3E00;
constexpr std::ptrdiff_t m_aimPunchAngle = 0x16FC;     // QAngle
constexpr std::ptrdiff_t m_aimPunchAngleVel = 0x1708;  // QAngle
constexpr std::ptrdiff_t m_aimPunchTickBase = 0x1714;  // GameTick_t
constexpr std::ptrdiff_t m_lifeState = 0x354;          // uint8
constexpr std::ptrdiff_t m_bPawnIsAlive = 0x904;       // bool
constexpr std::ptrdiff_t m_iDesiredFOV = 0x77C;



}  // namespace offsets
// namespace offsets
