#include "Config.h"
#include <thread>
#include <filesystem>>

Config cfg;

bool IsKeyDown(int VK)
{
    return (GetAsyncKeyState(VK) & 0x8000) != 0;
}

bool ConfigManager::CheckDir(char* dir)
{
    return std::filesystem::exists(dir) && std::filesystem::is_directory(dir);
}

std::vector<std::string> ConfigManager::GetFileList(const std::string& directoryPath)
{
    std::vector<std::string> iniFileList;

    for (const auto& entry : std::filesystem::directory_iterator(directoryPath))
    {
        if (entry.is_regular_file())
        {
            auto path = entry.path();
            if (path.extension() == ".ini")
                iniFileList.push_back(path.filename().string());
        }
    }

    return iniFileList;
}

void ConfigManager::SaveSetting(std::string filename)
{
    if (filename.empty())
        return;

    std::string path = ".\\Config\\" + filename;

    if (filename.find(".ini") == std::string::npos)
        path += ".ini";

    mINI::INIFile file(path.c_str());

    // config.iniの存在をチェック
    if (!std::filesystem::is_regular_file(path.c_str()))
        file.generate(ini, true);
    else
        file.read(ini);

    // AimBot
    ini["aimbot"]["enable"] = std::to_string(cfg.AimBot).c_str();
    ini["aimbot"]["aimatteam"] = std::to_string(cfg.AimAtTeam).c_str();
    ini["aimbot"]["vischeck"] = std::to_string(cfg.VisCheck).c_str();
    ini["aimbot"]["targetbone"] = std::to_string(cfg.AimTargetBone).c_str();
    ini["aimbot"]["drawfov"] = std::to_string(cfg.DrawFov).c_str();
    ini["aimbot"]["rainbowfov"] = std::to_string(cfg.RainbowFov).c_str();
    ini["aimbot"]["fovfilled"] = std::to_string(cfg.FovFilled).c_str();
    ini["aimbot"]["fov"] = std::to_string(cfg.AimFov).c_str();
    ini["aimbot"]["smooth"] = std::to_string(cfg.Smooth).c_str();
    ini["aimbot"]["distance"] = std::to_string(cfg.Aim_MaxDistance).c_str();
    ini["aimbot"]["aimtype"] = std::to_string(cfg.AimType).c_str();
    ini["aimbot"]["aimkeytyle"] = std::to_string(cfg.AimKeyType).c_str();

    // Visual
    ini["visual"]["enable"] = std::to_string(cfg.PlayerESP).c_str();
    ini["visual"]["team"] = std::to_string(cfg.TeamESP).c_str();
    ini["visual"]["vehicle"] = std::to_string(cfg.VehicleESP).c_str();
    ini["visual"]["box"] = std::to_string(cfg.ESP_Box).c_str();
    ini["visual"]["boxfilled"] = std::to_string(cfg.ESP_BoxFilled).c_str();
    ini["visual"]["line"] = std::to_string(cfg.ESP_Line).c_str();
    ini["visual"]["distance"] = std::to_string(cfg.ESP_Distance).c_str();
    ini["visual"]["name"] = std::to_string(cfg.ESP_Name).c_str();
    ini["visual"]["healthbar"] = std::to_string(cfg.ESP_HealthBar).c_str();
    ini["visual"]["skeleton"] = std::to_string(cfg.ESP_Skeleton).c_str();
    ini["visual"]["maxdistance"] = std::to_string(cfg.ESP_MaxDistance).c_str();
    ini["visual"]["boxtype"] = std::to_string(cfg.ESP_BoxType).c_str();

    // Misc
    ini["misc"]["streamproof"] = std::to_string(cfg.StreamProof).c_str();
    ini["misc"]["crosshair"] = std::to_string(cfg.Crosshair).c_str();
    ini["misc"]["crosshairtype"] = std::to_string(cfg.CrosshairType).c_str();
    ini["misc"]["rainbowrate"] = std::to_string(cfg.RainbowRate).c_str();
    ini["misc"]["unlockall"] = std::to_string(cfg.UnlockAll).c_str();
    ini["misc"]["swaymod"] = std::to_string(cfg.SwayModify).c_str();
    ini["misc"]["swayvalue"] = std::to_string(cfg.ModVal).c_str();
    ini["misc"]["damagehack"] = std::to_string(cfg.DamageHack).c_str();
    ini["misc"]["bps"] = std::to_string(cfg.ModBPS).c_str();

    // Keys
    ini["key"]["aim0"] = std::to_string(cfg.AimKey0).c_str();
    ini["key"]["aim1"] = std::to_string(cfg.AimKey1).c_str();

    file.write(ini);
}

void ConfigManager::LoadSetting(std::string filename)
{
    if (filename.empty())
        return;

    std::string path = ".\\Config\\" + filename;
    mINI::INIFile file(path.c_str());

    // config.iniの存在をチェック
    if (!std::filesystem::is_regular_file(path.c_str()))
        file.generate(ini, true);
    else
        file.read(ini);

    // AimBot
    std::istringstream(ini["aimbot"]["enable"]) >> cfg.AimBot;
    std::istringstream(ini["aimbot"]["aimatteam"]) >> cfg.AimAtTeam;
    std::istringstream(ini["aimbot"]["vischeck"]) >> cfg.VisCheck;
    std::istringstream(ini["aimbot"]["targetbone"]) >> cfg.AimTargetBone;
    std::istringstream(ini["aimbot"]["drawfov"]) >> cfg.DrawFov;
    std::istringstream(ini["aimbot"]["rainbowfov"]) >> cfg.RainbowFov;
    std::istringstream(ini["aimbot"]["fovfilled"]) >> cfg.FovFilled;
    std::istringstream(ini["aimbot"]["fov"]) >> cfg.AimFov;
    std::istringstream(ini["aimbot"]["smooth"]) >> cfg.Smooth;
    std::istringstream(ini["aimbot"]["distance"]) >> cfg.Aim_MaxDistance;
    std::istringstream(ini["aimbot"]["aimtype"]) >> cfg.AimType;
    std::istringstream(ini["aimbot"]["aimkeytyle"]) >> cfg.AimKeyType;

    // Visual
    std::istringstream(ini["visual"]["enable"]) >> cfg.PlayerESP;
    std::istringstream(ini["visual"]["team"]) >> cfg.TeamESP;
    std::istringstream(ini["visual"]["vehicle"]) >> cfg.VehicleESP;
    std::istringstream(ini["visual"]["box"]) >> cfg.ESP_Box;
    std::istringstream(ini["visual"]["boxfilled"]) >> cfg.ESP_BoxFilled;
    std::istringstream(ini["visual"]["line"]) >> cfg.ESP_Line;
    std::istringstream(ini["visual"]["distance"]) >> cfg.ESP_Distance;
    std::istringstream(ini["visual"]["name"]) >> cfg.ESP_Name;
    std::istringstream(ini["visual"]["healthbar"]) >> cfg.ESP_HealthBar;
    std::istringstream(ini["visual"]["skeleton"]) >> cfg.ESP_Skeleton;
    std::istringstream(ini["visual"]["maxdistance"]) >> cfg.ESP_MaxDistance;
    std::istringstream(ini["visual"]["boxtype"]) >> cfg.ESP_BoxType;

    // System
    std::istringstream(ini["system"]["streamproof"]) >> cfg.StreamProof;
    std::istringstream(ini["misc"]["crosshair"]) >> cfg.Crosshair;
    std::istringstream(ini["misc"]["crosshairtype"]) >> cfg.CrosshairType;
    std::istringstream(ini["misc"]["rainbowrate"]) >> cfg.RainbowRate;
    std::istringstream(ini["misc"]["unlockall"]) >> cfg.UnlockAll;
    std::istringstream(ini["misc"]["swaymod"]) >> cfg.SwayModify;
    std::istringstream(ini["misc"]["swayvalue"]) >> cfg.ModVal;
    std::istringstream(ini["misc"]["damagehack"]) >> cfg.DamageHack;
    std::istringstream(ini["misc"]["bps"]) >> cfg.ModBPS;

    // Keys
    std::istringstream(ini["key"]["aim0"]) >> cfg.AimKey0;
    std::istringstream(ini["key"]["aim1"]) >> cfg.AimKey1;
}
