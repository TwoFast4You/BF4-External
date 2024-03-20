#pragma once
#include "..\Utils\Memory\Memory.h"
#include <SimpleMath.h>
using namespace DirectX::SimpleMath;

namespace offset
{
    constexpr auto ClientgameContext = 0x142670D80;
    constexpr auto SyncBFSetting = 0x1423717C0;
    constexpr auto GameRenderer = 0x142672378;
    constexpr auto DxRenderer = 0x142738080;
    constexpr auto ClientWeapons = 0x1423B2EC8;

    constexpr auto PlayerManager = 0x60;
    constexpr auto LocalPlayer = 0x540;
    constexpr auto ClientPlayer = 0x548;
    constexpr auto ClientSoldier = 0x14D0;
    constexpr auto ClientVehicle = 0x14C0;

    constexpr auto PlayerTeam = 0x13CC;
    constexpr auto PlayerName = 0x1836;
    constexpr auto Occlude = 0x5B1;
    constexpr auto Spectator = 0x13C9;
}

struct AxisAlignedBox
{
    Vector4 Min;
    Vector4 Max;
};

class HealthComponent
{
public:
    char _0x0000[32];
    float m_Health; //0x0020 
    float m_MaxHealth; //0x0024 
    char _0x0028[16];
    float m_VehicleHealth; //0x0038 
    float m_MaxVehicleHealth; //0x0038 
};

class Player
{
public:
    uint64_t ClientPlayer;
    uint64_t ClientSoldier;
    uint64_t ClientVehicle;

    int Team;
    HealthComponent TmpComponent;
    HealthComponent* HealthBase = &TmpComponent;
    Vector3 Position;
    bool Occlude;
    int Pose;

    Matrix VehicleTranfsorm;
    AxisAlignedBox SoldierAABB;
    AxisAlignedBox VehicleAABB;

    bool InVehicle()
    {
        return ClientVehicle != NULL;
    }
    bool IsSpectator()
    {
        return m.Read<bool>(ClientPlayer + 0x13C9);
    }
    bool IsVisible()
    {
        return !Occlude;
    }
    bool IsDead()
    {
        return HealthBase->m_Health <= 0.f && Position == Vector3(0.f, 0.f, 0.f);
    }
    void Update()
    {
        ClientSoldier = m.Read<uint64_t>(ClientPlayer + offset::ClientSoldier);
        ClientVehicle = m.Read<uint64_t>(ClientPlayer + offset::ClientVehicle);

        // Team
        Team = m.Read<int>(ClientPlayer + offset::PlayerTeam);
        // Health
        uint64_t TmpHealthComponent = m.Read<uint64_t>(ClientSoldier + 0x140);
        TmpComponent = m.Read<HealthComponent>(TmpHealthComponent);
        // Position
        if (InVehicle()) // Vehicle
        {
            uint64_t DynamicPhysicsEntity = m.Read<uint64_t>(ClientVehicle + 0x238);

            if (DynamicPhysicsEntity)
            {
                uint64_t pPhysicsEntity = m.Read<uint64_t>(DynamicPhysicsEntity + 0xA0);
                VehicleTranfsorm = m.Read<Matrix>(pPhysicsEntity);
                VehicleAABB = m.Read<AxisAlignedBox>(ClientVehicle + 0x250);

                Position = Vector3(VehicleTranfsorm._41, VehicleTranfsorm._42, VehicleTranfsorm._43);
            }
        }
        else // Soldier
        {
            uint64_t TmpPosition = m.Read<uint64_t>(ClientSoldier + 0x490);
            Position = m.Read<Vector3>(TmpPosition + 0x30);
        }

        // Visible
        Occlude = m.Read<bool>(ClientSoldier + 0x5B1);
        Pose = m.Read<int>(ClientSoldier + 0x4F0);
    }

    AxisAlignedBox GetAABB()
    {
        AxisAlignedBox aabb = AxisAlignedBox();
        

        switch (Pose)
        {
        case 0:
            aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.Max = Vector4(0.350000f, 1.700000f, 0.350000f, 0);
            break;
        case 1:
            aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.Max = Vector4(0.350000f, 1.150000f, 0.350000f, 0);
            break;
        case 2:
            aabb.Min = Vector4(-0.350000f, 0.000000f, -0.350000f, 0);
            aabb.Max = Vector4(0.350000f, 0.400000f, 0.350000f, 0);
            break;
        default:
            break;
        }

        return aabb;
    }
};

extern float GetDistance(Vector3 value1, Vector3 value2);
extern Vector3 GetBone(uint64_t pSoldier, int bone_id);
extern bool WorldToScreen(const Vector3& WorldPos, Vector2& ScreenPos);
extern bool WorldToScreen(const Vector3& WorldPos, Vector3& ScreenPos);