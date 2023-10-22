#pragma once

namespace offset
{
    constexpr auto ClientgameContext = 0x142670D80;
    constexpr auto SyncBFSetting = 0x1423717C0;
    constexpr auto GameRenderer	= 0x142672378;
    constexpr auto DxRenderer	= 0x142738080;
    constexpr auto ClientWeapons = 0x1423B2EC8;

    constexpr auto PlayerManager = 0x60;
    constexpr auto LocalPlayer   = 0x540;
    constexpr auto ClientPlayer  = 0x548;
    constexpr auto ClientSoldier = 0x14D0;
    constexpr auto ClientVehicle = 0x14C0;

    constexpr auto PlayerTeam = 0x13CC;
    constexpr auto PlayerName = 0x1836;
    constexpr auto Occlude = 0x5B1;
    constexpr auto Spectator = 0x13C9;
}

class Player
{
public:
    uint64_t Player;
    uint64_t Soldier;
    uint64_t Vehicle;

    int Team;
    float Health;
    Vector3 Position;
    bool Occlude;
    float Yaw;

    bool IsSpectator()
    {
        return m.RPM<bool>(Player + 0x13C9);
    }
    bool IsVisible()
    {
        return !Occlude;
    }
    bool IsAlive()
    {
        return Health <= 0.f || Position == Vector3(0.f, 0.f, 0.f);
    }
};