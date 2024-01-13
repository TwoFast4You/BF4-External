#pragma once
#include "Memory\Memory.h"
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

    Matrix VehicleTranfsorm;
    AxisAlignedBox SoldierAABB;
    AxisAlignedBox VehicleAABB;

    bool InVehicle()
    {
        return ClientVehicle != NULL;
    }
    bool IsSpectator()
    {
        return m.RPM<bool>(ClientPlayer + 0x13C9);
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
        ClientSoldier = m.RPM<uint64_t>(ClientPlayer + offset::ClientSoldier);
        ClientVehicle = m.RPM<uint64_t>(ClientPlayer + offset::ClientVehicle);

        // Team
        Team = m.RPM<int>(ClientPlayer + offset::PlayerTeam);
        // Health
        uint64_t TmpHealthComponent = m.RPM<uint64_t>(ClientSoldier + 0x140);
        TmpComponent = m.RPM<HealthComponent>(TmpHealthComponent);
        // Position
        if (InVehicle()) // Vehicle
        {
            uint64_t DynamicPhysicsEntity = m.RPM<uint64_t>(ClientVehicle + 0x238);

            if (DynamicPhysicsEntity)
            {
                uint64_t pPhysicsEntity = m.RPM<uint64_t>(DynamicPhysicsEntity + 0xA0);
                VehicleTranfsorm = m.RPM<Matrix>(pPhysicsEntity);
                VehicleAABB = m.RPM<AxisAlignedBox>(ClientVehicle + 0x250);

                Position = Vector3(VehicleTranfsorm._41, VehicleTranfsorm._42, VehicleTranfsorm._43);
            }
        }
        else // Soldier
        {
            uint64_t TmpPosition = m.RPM<uint64_t>(ClientSoldier + 0x490);
            Position = m.RPM<Vector3>(TmpPosition + 0x30);
        }

        // Visible
        Occlude = m.RPM<bool>(ClientSoldier + 0x5B1);
    }

    AxisAlignedBox GetAABB()
    {
        AxisAlignedBox aabb = AxisAlignedBox();
        int Pose = m.RPM<int>(ClientSoldier + 0x4F0);

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

float GetDistance(Vector3 value1, Vector3 value2)
{
    float num = value1.x - value2.x;
    float num2 = value1.y - value2.y;
    float num3 = value1.z - value2.z;

    return sqrt(num * num + num2 * num2 + num3 * num3);
}

Vector3 GetBone(uint64_t pSoldier, int bone_id)
{
    Vector3 tmp, out;
    uint64_t ragdoll_component = m.RPM<uint64_t>(pSoldier + 0x580);
    if (!ragdoll_component)
        return Vector3(0, 0, 0);

    uint64_t quat = m.RPM<uint64_t>(ragdoll_component + 0xB0);
    if (!quat)
        return Vector3(0, 0, 0);

    tmp = m.RPM<Vector3>(quat + bone_id * 0x20);

    out.x = tmp.x;
    out.y = tmp.y;
    out.z = tmp.z;

    return out;
}

bool WorldToScreen(const Vector3& WorldPos, Vector2& ScreenPos)
{
    uint64_t GameRenderer = m.RPM<uint64_t>(offset::GameRenderer);
    uint64_t RenderView = m.RPM<uint64_t>(GameRenderer + 0x60);

    if (RenderView == 0)
        return false;

    uint64_t DXRenderer = m.RPM<uint64_t>(offset::DxRenderer);
    uint64_t m_pScreen = m.RPM<uint64_t>(DXRenderer + 0x38);

    if (m_pScreen == 0)
        return false;

    Matrix view_x_projection = m.RPM<Matrix>(RenderView + 0x420);

    int ScreenWidth = m.RPM<int>(m_pScreen + 0x58);
    int ScreenHeight = m.RPM<int>(m_pScreen + 0x5C);

    float cX = ScreenWidth * 0.5f;
    float cY = ScreenHeight * 0.5f;

    float w = view_x_projection(0, 3) * WorldPos.x + view_x_projection(1, 3) * WorldPos.y + view_x_projection(2, 3) * WorldPos.z + view_x_projection(3, 3);

    if (w < 0.65f)
        return false;

    float x = view_x_projection(0, 0) * WorldPos.x + view_x_projection(1, 0) * WorldPos.y + view_x_projection(2, 0) * WorldPos.z + view_x_projection(3, 0);
    float y = view_x_projection(0, 1) * WorldPos.x + view_x_projection(1, 1) * WorldPos.y + view_x_projection(2, 1) * WorldPos.z + view_x_projection(3, 1);

    ScreenPos.x = cX + cX * x / w;
    ScreenPos.y = cY - cY * y / w;

    return true;
}

bool WorldToScreen(const Vector3& WorldPos, Vector3& ScreenPos)
{
    uint64_t GameRenderer = m.RPM<uint64_t>(offset::GameRenderer);
    uint64_t RenderView = m.RPM<uint64_t>(GameRenderer + 0x60);

    if (RenderView == 0)
        return false;

    uint64_t DXRenderer = m.RPM<uint64_t>(offset::DxRenderer);
    uint64_t m_pScreen = m.RPM<uint64_t>(DXRenderer + 0x38);

    if (m_pScreen == 0)
        return false;

    Matrix view_x_projection = m.RPM<Matrix>(RenderView + 0x420);

    int ScreenWidth = m.RPM<int>(m_pScreen + 0x58);
    int ScreenHeight = m.RPM<int>(m_pScreen + 0x5C);

    float cX = ScreenWidth * 0.5f;
    float cY = ScreenHeight * 0.5f;

    float w = view_x_projection(0, 3) * WorldPos.x + view_x_projection(1, 3) * WorldPos.y + view_x_projection(2, 3) * WorldPos.z + view_x_projection(3, 3);

    if (w < 0.65f)
    {
        ScreenPos.z = w;
        return false;
    }

    float x = view_x_projection(0, 0) * WorldPos.x + view_x_projection(1, 0) * WorldPos.y + view_x_projection(2, 0) * WorldPos.z + view_x_projection(3, 0);
    float y = view_x_projection(0, 1) * WorldPos.x + view_x_projection(1, 1) * WorldPos.y + view_x_projection(2, 1) * WorldPos.z + view_x_projection(3, 1);

    ScreenPos.x = cX + cX * x / w;
    ScreenPos.y = cY - cY * y / w;
    ScreenPos.z = w;

    return true;
}
