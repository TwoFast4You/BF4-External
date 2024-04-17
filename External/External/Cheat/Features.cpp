#include "Cheat.h"
#include "SDK.h"

// mouse_event based aimbot
bool Cheat::AimBot(Vector2& TargetPosition)
{
    /*----| KeyChecks |--------------------------------------------------------------------------------*/
    switch (cfg.AimKeyType)
    {
    case 0: // and
        if (cfg.AimKey1 == NULL && IsKeyDown(cfg.AimKey0))
            break;
        else if (!IsKeyDown(cfg.AimKey0) || !IsKeyDown(cfg.AimKey1))
            return false;
        else if (!IsKeyDown(cfg.AimKey0))
            return false;
        break;
    case 1: // or
        if (cfg.AimKey1 == NULL && IsKeyDown(cfg.AimKey0))
            break;
        else if (IsKeyDown(cfg.AimKey0) || IsKeyDown(cfg.AimKey1))
            break;

        break;
    }
    /*--------------------------------------------------------------------------------------------------*/

    Vector2 ScreenMiddle = { (float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f };

    // AimBot実行?
    if (TargetPosition != Vector2(0.f, 0.f))
    {
        int cX = (ScreenMiddle.x - TargetPosition.x) / cfg.Smooth;
        int cY = (ScreenMiddle.y - TargetPosition.y) / cfg.Smooth;

        mouse_event(MOUSEEVENTF_MOVE, -cX, -cY, 0, 0);
    }
}

void Cheat::Misc()
{
    while (cfg.Run)
    {
        uint64_t SyncBFSetting = m.Read<uint64_t>(offset::SyncBFSetting);

        // UnlockAll
        if (cfg.UnlockAll && SyncBFSetting != 0 && m.Read<bool>(SyncBFSetting + 0x54) == false)
            m.Write<bool>(SyncBFSetting + 0x54, true);

        uint64_t Weapon = m.Read<uint64_t>(offset::ClientWeapons);

        if (!Weapon)
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

        Sleep(10);
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