#include "Cheat.h"
#include "SDK.h"
#include "..\Overlay\Overlay.h"
#include "..\Utils\Image\NaziRussia.h"
#include "..\ImGui\Font\RobotoLight.h"
#include "..\ImGui\Font\RobotoMedium.h"
#include <WICTextureLoader.h>
#include <filesystem>
using namespace DirectX;

// Menu String
const char* BoxList[] = { "2D Box", "2D Corner Box" };
const char* BoneList[] = { "Head", "Spine" };
const char* bAimTupeText[] = { "Crosshair", "Distance" };
const char* SkeletonColorModeList[] = { "ESP", "User" };
const char* CrosshairList[] = { "Cross", "Circle" };
const char* AimKeyTypeList[] = { "and", "or" };

// Resource
ID3D11ShaderResourceView* LogoImage = nullptr;

// Functions
void LoadImageByMemory(ID3D11Device* device, unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result);

// Config
ConfigManager cfmg;
char ConfigPath[] = ".\\Config\\";

bool Cheat::Init()
{
    // LoadFonts
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.Fonts->AddFontFromMemoryCompressedTTF(RobotoLight_compressed_data, RobotoLight_compressed_size, 15.f, nullptr);
    io.Fonts->Build();

    // 画像をロード
    LoadImageByMemory(g_pd3dDevice, NaziRawData, sizeof(NaziRawData), &LogoImage);

    game = FindWindowA(nullptr, "Battlefield 4");

    // Configフォルダの存在をチェック
    if (!cfmg.CheckDir(ConfigPath))
        system("mkdir Config");

    return true;
}

void Cheat::RenderInfo()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)cfg.GameRect.right, (float)cfg.GameRect.bottom));
    ImGui::Begin("##Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::TextColored(ImVec4(1.f, 1.f, 1.f, 1.f), "%.f FPS", ImGui::GetIO().Framerate);

    // AimFov
    if (cfg.AimBot && cfg.DrawFov)
    {
        ImColor fovcol = cfg.RainbowFov ? Rainbow : FOV_User;

        Circle((float)cfg.GameRect.right, (float)cfg.GameRect.bottom, cfg.AimFov + 1.f, fovcol, 1.0f);
        if (cfg.FovFilled)
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), cfg.AimFov, ImColor(fovcol.Value.x, fovcol.Value.y, fovcol.Value.z, 0.1f), NULL);
    }

    // Crosshair
    if (cfg.Crosshair)
    {
        switch (cfg.CrosshairType)
        {
        case 0:
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f + 4), ((float)cfg.GameRect.bottom / 2.f)), ImVec2(((float)cfg.GameRect.right / 2.f + 10), ((float)cfg.GameRect.bottom / 2.f)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f - 4), ((float)cfg.GameRect.bottom / 2.f)), ImVec2(((float)cfg.GameRect.right / 2.f - 10), ((float)cfg.GameRect.bottom / 2.f)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f + 4)), ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f + 10)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            DrawLine(ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f - 4)), ImVec2(((float)cfg.GameRect.right / 2.f), ((float)cfg.GameRect.bottom / 2.f - 10)), ImColor(1.f, 1.f, 1.f, 1.f), 1);
            break;
        case 1:
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), 3, ImColor(0.f, 0.f, 0.f, 1.f), NULL);
            ImGui::GetBackgroundDrawList()->AddCircleFilled(ImVec2((float)cfg.GameRect.right / 2.f, (float)cfg.GameRect.bottom / 2.f), 2, ImColor(1.f, 1.f, 1.f, 1.f), NULL);
            break;
        default:
            break;
        }
    }

    ImGui::End();
}

void Cheat::RenderMenu()
{
    static int menu = 0;

    ImGui::SetNextWindowSize(ImVec2(750.f, 500.f));
    ImGui::Begin("ProjectLocker - Battlefield4 [ EXTERNAL ]", &cfg.ShowMenu, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

    //---// Clild 0 //-----------------------------------//
    ImGui::BeginChild("##BaseChild", ImVec2(150.f, ImGui::GetContentRegionAvail().y), false);

    // Icon
    ImGui::BeginChild("##TitleChild", ImVec2(ImGui::GetContentRegionAvail().x, 150.f), true);
    ImGui::Image((void*)LogoImage, ImGui::GetContentRegionAvail()); // size : 134 * 134
    ImGui::EndChild();

    ImGui::Spacing();

    ImGui::BeginChild("##SomeChild", ImVec2(ImGui::GetContentRegionAvail()), true);

    ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y - 130.f);

    /*---| Config |----------------------------------*/ 
    ImGui::SeparatorText("Config");
    
    static int FileNum = 0;
    static char InputName[12];

    // Configフォルダ内の.iniファイルを取得、リスト化
    auto vec = cfmg.GetFileList(ConfigPath); 
    const char** FileList = new const char* [vec.size()];

    for (size_t j = 0; j < vec.size(); j++)
        FileList[j] = vec[j].c_str();

    ImGui::InputText("Name", InputName, IM_ARRAYSIZE(InputName));
    ImGui::Combo("##CfgList", &FileNum, FileList, vec.size());

    // Button
    if (ImGui::Button("Save", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 4.f, 20.f)))
    {
        if (vec.size() == 0 && InputName[0] != NULL || vec.size() != 0)
            std::thread([&]() {cfmg.SaveSetting(InputName[0] != NULL ? InputName : FileList[FileNum]); }).join();

        ZeroMemory(InputName, sizeof(InputName));
    }
    ImGui::SameLine();
    if (ImGui::Button("Load", ImVec2(ImGui::GetContentRegionAvail().x, 20.f)) && vec.size() != 0)
        std::thread([&]() {cfmg.LoadSetting(FileList[FileNum]); }).join();

    delete[] FileList;
    /*-----------------------------------------------*/

    // Exit
    ImGui::Separator();
    ImGui::Spacing();
    if (ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvail().x, 30.f)))
        cfg.Run = false;

    ImGui::EndChild();
    ImGui::EndChild();
    //---// Clild 0 //-----------------------------------//

    ImGui::SameLine();

    //---// Clild 1 //-----------------------------------//
    ImGui::BeginChild("BaseChild##2", ImVec2(ImGui::GetContentRegionAvail()));

    ImGuiStyle& style = ImGui::GetStyle();
    auto FramePadding = style.FramePadding;
    style.FramePadding = ImVec2(40, 8);

    if (ImGui::BeginTabBar("##ContextTabBar"))
    {
        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("   AimBot   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftAimBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("AimBot", &cfg.AimBot);
            ImGui::Checkbox("Aim at Team", &cfg.AimAtTeam);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  AimBot Config");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Visibility Check", &cfg.VisCheck);
            ImGui::Combo("AimBone", &cfg.AimTargetBone, BoneList, IM_ARRAYSIZE(BoneList));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  FOV");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("Draw FOV", &cfg.DrawFov);
            ImGui::Checkbox("Rainbow FOV", &cfg.RainbowFov);
            ImGui::Checkbox("Fov Filled", &cfg.FovFilled);
            ImGui::SliderFloat("FOV", &cfg.AimFov, 25.f, 650.f);
            ImGui::ColorEdit4("FOV Color", &FOV_User.Value.x);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightAimBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  AimBot Setting");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SliderFloat("FOV", &cfg.AimFov, 50.f, 650.f);
            ImGui::SliderInt("Smooth", &cfg.Smooth, 1, 20);
            ImGui::SliderFloat("Distance", &cfg.Aim_MaxDistance, 15.f, 600.f);
            ImGui::Combo("AimType", &cfg.AimType, bAimTupeText, IM_ARRAYSIZE(bAimTupeText));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  KeyBind");
            ImGui::Separator();
            ImGui::Spacing();

            const char* text1 = KeyNames[cfg.AimKey0];
            const char* text2 = KeyNames[cfg.AimKey1];

            if (cfg.KeyBinding)
            {
                switch (cfg.BindingID)
                {
                case 1:
                    text1 = "< Press Any Key >";
                    break;
                case 2:
                    text2 = "< Press Any Key >";
                    break;
                default:
                    break;
                }
            }

            if (ImGui::Button(text1, ImVec2(215.f, 20.f)))
            {
                cfg.KeyBinding = true;
                cfg.BindingID = 1;
                std::thread([&]() {this->KeyBinder(cfg.AimKey0); }).detach();
            }

            ImGui::PushItemWidth(215.f);
            ImGui::Combo("##KeyType", &cfg.AimKeyType, AimKeyTypeList, IM_ARRAYSIZE(AimKeyTypeList));
            ImGui::PopItemWidth();

            if (ImGui::Button(text2, ImVec2(215.f, 20.f)))
            {
                cfg.KeyBinding = true;
                cfg.BindingID = 2;
                std::thread([&]() {this->KeyBinder(cfg.AimKey1); }).detach();
            }

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("   Visual   "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftVisualBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  Visual");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Player ESP", &cfg.PlayerESP);
            ImGui::Checkbox("Team ESP", &cfg.TeamESP);
            ImGui::Checkbox("Vehicle ESP", &cfg.VehicleESP);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Options");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Box", &cfg.ESP_Box);
            ImGui::Checkbox("BoxFilled", &cfg.ESP_BoxFilled);
            ImGui::Checkbox("Line", &cfg.ESP_Line);
            ImGui::Checkbox("Distance", &cfg.ESP_Distance);
            ImGui::Checkbox("Name", &cfg.ESP_Name);
            ImGui::Checkbox("HealthBar", &cfg.ESP_HealthBar);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightVisualBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  ESP Setting");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SliderFloat("Distance", &cfg.ESP_MaxDistance, 25.f, 2000.f);
            ImGui::Combo("Box Style", &cfg.ESP_BoxType, BoxList, IM_ARRAYSIZE(BoxList));

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  ESP Colors");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::ColorEdit4("Normal", &ESP_Normal.Value.x);
            ImGui::ColorEdit4("Visible", &ESP_Visible.Value.x);
            ImGui::ColorEdit4("Team", &ESP_Team.Value.x);
            ImGui::ColorEdit4("BoxFilled", &ESP_Filled.Value.x);
            ImGui::ColorEdit4("Skeleton", &ESP_Skeleton.Value.x);
            ImGui::Combo("Skeleton Color", &cfg.ESP_SkeletonColor, SkeletonColorModeList, IM_ARRAYSIZE(SkeletonColorModeList));

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("    Misc    "))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftMiscBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 12.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  System");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("StreamProof", &cfg.StreamProof);
            ImGui::Checkbox("Crosshair", &cfg.Crosshair);
            ImGui::Combo("Type", &cfg.CrosshairType, CrosshairList, IM_ARRAYSIZE(CrosshairList));
            ImGui::SliderInt("RainbowRate", &cfg.RainbowRate, 1, 200);

            ImGui::Text("  Game");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("SwayModify", &cfg.SwayModify);
            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightMiscBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("   SwayModify");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Checkbox("SwayModify", &cfg.SwayModify);
            ImGui::SliderFloat("Recoil/Spread", &cfg.ModVal, 0.f, 1.f);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("   DamageHack");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::SliderInt("BulletPerShell", &cfg.ModBPS, 1, 30);
            ImGui::Text("CurrentBPS : %d", cfg.CurrentBPS);
            if (ImGui::Button("Apply"))
                std::thread([&]() { this->SetBPS(cfg.ModBPS); }).detach();

            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = ImVec2(40, 8);
        if (ImGui::BeginTabItem("Developer"))
        {
            /*---------------*/
            style.FramePadding = FramePadding;
            ImGui::Spacing();
            ImGui::BeginChild("##LeftDevBase", ImVec2(ImGui::GetContentRegionAvail().x / 2.f - 8.f, ImGui::GetContentRegionAvail().y), false);

            ImGui::Text("  Process");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("PID : %d", m.PID);

            ImGui::NewLine();
            ImGui::Spacing();

            ImGui::Text("  Contact Developer");
            ImGui::Separator();
            ImGui::Spacing();
            ImGui::Text("Twitter : @WallHax_Ena");
            ImGui::Text("Discord : wallhax_ena");
            ImGui::NewLine();
            if (ImGui::Button("Open Twitter"))
                ShellExecuteA(NULL, "open", "https://twitter.com/WallHax_Ena", NULL, NULL, SW_SHOWDEFAULT);

            ImGui::EndChild();
            /*---------------*/
            ImGui::SameLine();
            /*---------------*/
            ImGui::BeginChild("##RightDevBase", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), false);



            ImGui::EndChild();
            /*---------------*/

            ImGui::EndTabItem();
        }

        style.FramePadding = FramePadding;
        ImGui::EndTabBar();
    }

    ImGui::EndChild();
    //---// Clild 1 //-----------------------------------//

    ImGui::End();
}

void Cheat::RenderESP()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)cfg.GameRect.right, (float)cfg.GameRect.bottom));
    ImGui::Begin("##ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

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
    for (int i = 0; i < 72; i++)
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
        else if (pEntity->ClientVehicle == pLocal->ClientVehicle && pLocal->ClientVehicle != 0 && pEntity->ClientVehicle != 0)
            continue;

        // GetDistance
        float distance = GetDistance(pLocal->Position, pEntity->Position);

        // Check ESP MaxDistance
        if (cfg.ESP_MaxDistance < distance)
            continue;

        // Vehicle ESP
        if (cfg.VehicleESP && pEntity->InVehicle())
        {
            if (!cfg.TeamESP && pEntity->Team == pLocal->Team)
                continue;

            DrawAABB(pEntity->VehicleAABB, pEntity->VehicleTranfsorm, pEntity->Team == pLocal->Team ? ESP_Team : ESP_Normal);

            if (cfg.ESP_Distance)
            {
                // float to Text
                std::string text = std::to_string((int)distance) + "m";
                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

                Vector2 VehicleScreen = Vector2();
                if (!WorldToScreen(pEntity->Position, VehicleScreen))
                    continue;

                if (VehicleScreen != Vector2(0.f, 0.f))
                    String(ImVec2(VehicleScreen.x - (textSize.x / 2.f), VehicleScreen.y), ImColor(1.f, 1.f, 1.f, 1.f), text.c_str());
            }

            continue;
        }
        else if (pEntity->InVehicle())
        {
            continue;
        }

        // Some check
        if (pEntity->IsDead() || pEntity->InVehicle())
            continue;
        else if (!cfg.TeamESP && pEntity->Team == pLocal->Team)
            continue;

        // WorldToScreen
        Vector2 ScreenPosition = Vector2(0.f, 0.f);
        if (!WorldToScreen(pEntity->Position, ScreenPosition))
            continue;

        // W2S Check
        if (ScreenPosition != Vector2(0.f, 0.f))
        {
            // Set ESP color
            ImColor color = pEntity->IsVisible() ? ESP_Visible : ESP_Normal;

            // Teammte
            if (cfg.TeamESP && pEntity->Team == pLocal->Team)
                color = ESP_Team;

            // Get some size.
            Vector3 Top = pEntity->Position + pEntity->GetAABB().Max;
            Vector3 Btm = pEntity->Position + pEntity->GetAABB().Min;

            Vector2 BoxTop, BoxBtm;
            if (!WorldToScreen(Top, BoxTop) || !WorldToScreen(Btm, BoxBtm))
                continue;
            else if (BoxTop == Vector2(0.f, 0.f) || BoxBtm == Vector2(0.f, 0.f))
                continue;

            float BoxMiddle = ScreenPosition.x;
            float Height = BoxBtm.y - BoxTop.y;
            float Width = Height / 4.f;

            // Box
            if (cfg.ESP_Box)
            {
                DrawBox(ScreenPosition.x - Width, ScreenPosition.y, Height / 2.f, -Height, color);

                // Filled
                if (cfg.ESP_BoxFilled)
                    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(BoxMiddle - Width, ScreenPosition.y), ImVec2(BoxMiddle + Width, ScreenPosition.y - Height), ImColor(0.f, 0.f, 0.f, 0.35f), NULL);
            }

            // Line
            if (cfg.ESP_Line)
                DrawLine(ImVec2(cfg.GameRect.right / 2, cfg.GameRect.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), color, 1);

            // Bone ESP
            if (cfg.ESP_Skeleton)
            {
                int aSkeleton[][2] =
                {
                { 104, 142 },{ 142, 9 },{ 9, 11 },{ 11, 15 },
                { 142,109},{109,111 },{111, 115},{ 142, 5 },
                { 5,  188},{ 5, 197},{ 188, 184},{ 197, 198},
                };

                for (int j = 0; j < 12; ++j)
                {
                    Vector3 Bone1 = pEntity->GetBone(aSkeleton[j][0]);
                    Vector3 Bone2 = pEntity->GetBone(aSkeleton[j][1]);

                    if (Bone1 == Vector3(0.f, 0.f, 0.f) || Bone2 == Vector3(0.f, 0.f, 0.f))
                        break;

                    Vector2 Out1, Out2;
                    if (WorldToScreen(Bone1, Out1) && WorldToScreen(Bone2, Out2))
                    {
                        if (Out1.x > cfg.GameRect.right || Out1.x < cfg.GameRect.left)
                            break;
                        else if (Out1 == Vector2(0.f, 0.f) || Out2 == Vector2(0.f, 0.f))
                            continue;

                        DrawLine(ImVec2(Out1.x, Out1.y), ImVec2(Out2.x, Out2.y), cfg.ESP_SkeletonColor == 0 ? color : ESP_Skeleton, 1);
                    }
                }
            }

            // Health Bar
            if (cfg.ESP_HealthBar)
                HealthBar(ScreenPosition.x - Width - 5, ScreenPosition.y + 1, 2.f, -Height - 1, pEntity->HealthBase->m_Health, pEntity->HealthBase->m_MaxHealth, true);

            // Distance
            if (cfg.ESP_Distance)
            {
                // float to Text
                std::string text = std::to_string((int)distance) + "m";
                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

                String(ImVec2(ScreenPosition.x - (textSize.x / 2.f), ScreenPosition.y), ImColor(1.f, 1.f, 1.f, 1.f), text.c_str());
            }
        
            // Name
            if (cfg.ESP_Name)
            {
                char pName[128];
                m.ReadString(pEntity->ClientPlayer + offset::PlayerName, &pName, sizeof(pName));
                String(ImVec2(BoxTop.x - (ImGui::CalcTextSize(pName).x / 2.f), (BoxTop.y - ImGui::CalcTextSize(pName).y) - 2.f), ImColor(1.f, 1.f, 1.f, 1.f), pName);
            }
        }

        ZeroMemory(pEntity, sizeof(pEntity));
    }

    ZeroMemory(pLocal, sizeof(pLocal));

    ImGui::End();
}

void LoadImageByMemory(ID3D11Device* device, unsigned char* image, size_t image_size, ID3D11ShaderResourceView** result)
{
    CreateWICTextureFromMemory(device, image, image_size, nullptr, result);
}

void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

Vector3 Multiply(Vector3 vector, Matrix mat)
{
    return Vector3(mat._11 * vector.x + mat._21 * vector.y + mat._31 * vector.z,
        mat._12 * vector.x + mat._22 * vector.y + mat._32 * vector.z,
        mat._13 * vector.x + mat._23 * vector.y + mat._33 * vector.z);
}

void Cheat::DrawAABB(AxisAlignedBox aabb, Matrix tranform, ImColor color)
{
    Vector3 m_Position = Vector3(tranform._41, tranform._42, tranform._43);
    Vector3 fld = Multiply(Vector3(aabb.Min.x, aabb.Min.y, aabb.Min.z), tranform) + m_Position;
    Vector3 brt = Multiply(Vector3(aabb.Max.x, aabb.Max.y, aabb.Max.z), tranform) + m_Position;
    Vector3 bld = Multiply(Vector3(aabb.Min.x, aabb.Min.y, aabb.Max.z), tranform) + m_Position;
    Vector3 frt = Multiply(Vector3(aabb.Max.x, aabb.Max.y, aabb.Min.z), tranform) + m_Position;
    Vector3 frd = Multiply(Vector3(aabb.Max.x, aabb.Min.y, aabb.Min.z), tranform) + m_Position;
    Vector3 brb = Multiply(Vector3(aabb.Max.x, aabb.Min.y, aabb.Max.z), tranform) + m_Position;
    Vector3 blt = Multiply(Vector3(aabb.Min.x, aabb.Max.y, aabb.Max.z), tranform) + m_Position;
    Vector3 flt = Multiply(Vector3(aabb.Min.x, aabb.Max.y, aabb.Min.z), tranform) + m_Position;

    if (!WorldToScreen(fld, fld) || !WorldToScreen(brt, brt)
        || !WorldToScreen(bld, bld) || !WorldToScreen(frt, frt)
        || !WorldToScreen(frd, frd) || !WorldToScreen(brb, brb)
        || !WorldToScreen(blt, blt) || !WorldToScreen(flt, flt))
        return;

    DrawLine(ImVec2(fld.x, fld.y), ImVec2(flt.x, flt.y), color, 1.f);
    DrawLine(ImVec2(flt.x, flt.y), ImVec2(frt.x, frt.y), color, 1.f);
    DrawLine(ImVec2(frt.x, frt.y), ImVec2(frd.x, frd.y), color, 1.f);
    DrawLine(ImVec2(frd.x, frd.y), ImVec2(fld.x, fld.y), color, 1.f);
    DrawLine(ImVec2(bld.x, bld.y), ImVec2(blt.x, blt.y), color, 1.f);
    DrawLine(ImVec2(blt.x, blt.y), ImVec2(brt.x, brt.y), color, 1.f);
    DrawLine(ImVec2(brt.x, brt.y), ImVec2(brb.x, brb.y), color, 1.f);
    DrawLine(ImVec2(brb.x, brb.y), ImVec2(bld.x, bld.y), color, 1.f);
    DrawLine(ImVec2(fld.x, fld.y), ImVec2(bld.x, bld.y), color, 1.f);
    DrawLine(ImVec2(frd.x, frd.y), ImVec2(brb.x, brb.y), color, 1.f);
    DrawLine(ImVec2(flt.x, flt.y), ImVec2(blt.x, blt.y), color, 1.f);
    DrawLine(ImVec2(frt.x, frt.y), ImVec2(brt.x, brt.y), color, 1.f);
}
