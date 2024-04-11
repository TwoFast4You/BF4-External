#include "Cheat.h"
#include "SDK.h"

// 104 -> Head // 6 -> Chest(spine)
int AimBone = 104;

// mouse_event based aimbot
void Cheat::AimBot()
{
    while (cfg.Run)
    {
        /*----| SomeChecks |--------------------------------------------------------------------------------*/
        if (!cfg.AimBot)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            continue;
        }
        else if (!IsKeyDown(cfg.AimKey0) && !IsKeyDown(cfg.AimKey1))
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            continue;
        }

        bool check = false;

        switch (cfg.AimKeyType)
        {
        case 0: // and
            if (cfg.AimKey1 != 0)
                if (!IsKeyDown(cfg.AimKey0) || !IsKeyDown(cfg.AimKey1)) // どちらかが押されていなければ
                    continue;
                else
                    if (!IsKeyDown(cfg.AimKey0))
                        continue;

            check = true;
            break;
        case 1: // or
            if (cfg.AimKey1 != 0)
                if (IsKeyDown(cfg.AimKey0) || IsKeyDown(cfg.AimKey1))
                    check = true;
                else
                    if (!IsKeyDown(cfg.AimKey0))
                        continue;

            check = true;
            break;
        default:
            break;
        }

        if (!check)
            continue;
        /*--------------------------------------------------------------------------------------------------*/

        if (GetForegroundWindow() != game)
            continue;

        switch (cfg.AimTargetBone)
        {
        case 0:
            AimBone = 104;
            break;
        case 1:
            AimBone = 6;
            break;
        default:
            break;
        }

        float fov = 0.f;
        float MinFov = 0.f;
        float MinDistance = 0.f;
        Vector2 ScreenMiddle = { (float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f };

        // TargetPosition
        Vector2 TargetPosition = {};
        Player player, * pEntity = &player;
        Player local, * pLocal = &local;

        // Context
        uint64_t ClientGameContext = m.Read<uint64_t>(offset::ClientgameContext);
        uint64_t PlayerManager = m.Read<uint64_t>(ClientGameContext + offset::PlayerManager);
        uint64_t PlayerEntity = m.Read<uint64_t>(PlayerManager + offset::ClientPlayer);

        // LocalPlayer
        pLocal->ClientPlayer = m.Read<uint64_t>(PlayerManager + offset::LocalPlayer);
        pLocal->Update();

        // ESP Loop
        for (int i = 0; i < 64; i++)
        {
            // LocalPlayer Check
            if (pLocal->IsDead() && !pLocal->InVehicle())
                break;

            // Update Player
            pEntity->ClientPlayer = m.Read<uint64_t>(PlayerEntity + (i * 0x08));
            pEntity->Update();

            // Spectaror Warning
            if (pEntity->IsSpectator())
                continue;

            // Invalid Player
            if (pEntity->ClientPlayer == NULL)
                continue;
            else if (pEntity->ClientPlayer == pLocal->ClientPlayer)
                continue;
            else if (pEntity->IsDead() || pEntity->InVehicle())
                continue;
            else if (!cfg.AimAtTeam && pEntity->Team == pLocal->Team)
                continue;
            else if (!pEntity->IsVisible())
                continue;

            // GetDistance
            float distance = GetDistance(pLocal->Position, pEntity->Position);

            // CheckDistance
            if (cfg.Aim_MaxDistance < distance)
                continue;

            // GetBone Position
            Vector2 ScreenPosition = Vector2(0.f, 0.f);

            // ToDo : Prediction

            if (!WorldToScreen(pEntity->GetBone(AimBone), ScreenPosition))
                continue;

            // Fov check
            fov = abs((ScreenMiddle - ScreenPosition).Length());
            
            if (fov < cfg.AimFov)
            {
                switch (cfg.AimType)
                {
                case 0:
                    if (MinFov == 0.f || MinFov > fov)
                    {
                        MinFov = fov;
                        TargetPosition = ScreenPosition;
                    }
                    break;
                case 1:
                    if (MinDistance == 0.f || MinDistance > distance)
                    {
                        MinDistance = distance;
                        TargetPosition = ScreenPosition;
                    }
                    break;
                default:
                    break;
                }
            }
        }

        // AimBot実行?
        if (TargetPosition != Vector2(0.f, 0.f))
        {
            int cX = (ScreenMiddle.x - TargetPosition.x) / cfg.Smooth;
            int cY = (ScreenMiddle.y - TargetPosition.y) / cfg.Smooth;

            mouse_event(MOUSEEVENTF_MOVE, -cX, -cY, 0, 0);
        }
    }
}

void Cheat::Misc()
{
    while (cfg.Run)
    {
        uint64_t SyncBFSetting = m.Read<uint64_t>(offset::SyncBFSetting);
        uint64_t Weapon = m.Read<uint64_t>(offset::ClientWeapons);

        if (!Weapon || !SyncBFSetting)
        {
            Sleep(10);
            continue;
        }

        uint64_t weaponPtr = m.Read<uint64_t>(Weapon + 0x128);
        uint64_t GunSwayData = m.Read<uint64_t>(weaponPtr + 0x30); // Sway
        uint64_t FiringFunctionData = m.Read<uint64_t>(weaponPtr + 0x10);
        cfg.CurrentBPS = m.Read<int>(FiringFunctionData + 0xD8);

        // Recoil / Spread
        if (cfg.SwayModify && GunSwayData != 0 && m.Read<float>(GunSwayData + 0x430) != cfg.ModVal)
        {
            m.Write<float>(GunSwayData + 0x430, cfg.ModVal);
            m.Write<float>(GunSwayData + 0x438, cfg.ModVal);
            m.Write<float>(GunSwayData + 0x434, cfg.ModVal);
            m.Write<float>(GunSwayData + 0x43C, cfg.ModVal);
        }
        else if (!cfg.SwayModify && GunSwayData != 0 && m.Read<float>(GunSwayData + 0x430) != 1.f)
        {
            m.Write<float>(GunSwayData + 0x430, 1.f);
            m.Write<float>(GunSwayData + 0x438, 1.f);
            m.Write<float>(GunSwayData + 0x434, 1.f);
            m.Write<float>(GunSwayData + 0x43C, 1.f);
        }

        // UnlockAll
        if (cfg.UnlockAll && SyncBFSetting != 0 && m.Read<bool>(SyncBFSetting + 0x54) == false)
            m.Write<bool>(SyncBFSetting + 0x54, true);

        Sleep(1);
    }
}

bool Cheat::SetBPS(int value)
{
    uint64_t Weapon = m.Read<uint64_t>(offset::ClientWeapons);

    if (!Weapon)
        return false;

    uint64_t weaponPtr = m.Read<uint64_t>(Weapon + 0x128);
    uint64_t FiringFunctionData = m.Read<uint64_t>(weaponPtr + 0x10);

    if (!FiringFunctionData)
        return false;

    m.Write<int>(FiringFunctionData + 0xD8, value);
    

    return true;
}

void Cheat::KeyBinder(int &target_key)
{
    // チェック用変数
    bool flag = false;

    // KeyBinder
    while (true)
    {
        for (int i = 0; i < 0x87; i++)
        {
            if (i == VK_LWIN || i == VK_RWIN)
                continue;

            if (IsKeyDown(i))
            {
                if (i == VK_ESCAPE)
                {
                    target_key = NULL;
                    flag = true;
                }  
                else
                {
                    target_key = i;
                    flag = true;
                }

                break;
            }
        }

        if (flag)
            break;
    }

    // Check
    if (cfg.AimKey0 == cfg.AimKey1)
        cfg.AimKey1 = 0;

    cfg.KeyBinding = false;
    cfg.BindingID = 0;
}

void Cheat::RainbowGenerator()
{
    while (cfg.Run)
    {
        static float rainbow;
        rainbow += 0.01f;
        if (rainbow > 1.f)
            rainbow = 0.f;

        constexpr float factor = 6.0f;
        int i = static_cast<int>(rainbow * factor);
        float f = rainbow * factor - i;
        float q = 1 - f;
        float t = 1 - (1 - f);

        Rainbow.Value.x = (i % 6 == 0) ? 1.f : (i % 6 == 1) ? q : (i % 6 == 2) ? 0 : (i % 6 == 3) ? 0 : (i % 6 == 4) ? t : 1.f;
        Rainbow.Value.y = (i % 6 == 0) ? t : (i % 6 == 1) ? 1.f : (i % 6 == 2) ? 1.f : (i % 6 == 3) ? q : (i % 6 == 4) ? 0 : 0;
        Rainbow.Value.z = (i % 6 == 0) ? 0 : (i % 6 == 1) ? 0 : (i % 6 == 2) ? t : (i % 6 == 3) ? 1.f : (i % 6 == 4) ? 1.f : q;

        std::this_thread::sleep_for(std::chrono::milliseconds(cfg.RainbowRate));
    }
}