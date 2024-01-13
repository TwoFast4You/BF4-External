#include "core.h"
#include "Utils\NotSDK.h"

extern bool Run;
extern bool ShowMenu;
extern bool Spectator;
extern bool StreamProof;
extern bool AutoWindowFocus;
int TabID = 0;
const char* TabList[] = { "Visual", "Misc", "Developer" };
int SklColorMode = 0;
const char* SklList[] = { "ESP Color", "User Color" };

void Core::Info()
{
    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(300.f, 100.f));
    ImGui::Begin("##BF4-Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    // Time
    time_t t = time(nullptr);
    struct tm nw;
    errno_t nTime = localtime_s(&nw, &t);
    ImGui::Text("%d:%d:%d", nw.tm_hour, nw.tm_min, nw.tm_sec);

    ImGui::End();
}

void Core::SpectatorList()
{
    ImGui::SetNextWindowBgAlpha(0.3f);
    
    static bool run = false;
    if (!run)
    {
        ImGui::SetNextWindowPos(ImVec2(16.f, 16.f));
        ImGui::SetNextWindowSize(ImVec2(275.f, 140.f));

        run = true;
    }

    ImGui::Begin("Spectator List", &Spectator, ImGuiWindowFlags_NoCollapse);

    for (int j = 0; j < NameList.size(); j++)
        ImGui::Text("%s", NameList[j].c_str());

    ImGui::End();
}

void Core::Menu()
{
    ImGui::SetNextWindowBgAlpha(0.99f);
    ImGui::SetNextWindowSize(ImVec2(600.f, 500.f));
    ImGui::Begin("ProjectLocker [ EXTERNAL ]", &ShowMenu, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    float TabButtonSize = ImGui::GetContentRegionAvail().x;

    // Tab
    for (int i = 0; i < 3; i++)
    {
        if (ImGui::Button(TabList[i], ImVec2(TabButtonSize / 3.f - 6.f, 35.f)))
            TabID = i;

        if (i < 2)
            ImGui::SameLine();
    }

    ImGui::Separator();

    // Right-Cild
    ImGui::BeginChild("##RightChild", ImVec2(ImGui::GetContentRegionAvail().x / 2.f, ImGui::GetContentRegionAvail().y), ImGuiWindowFlags_ChildMenu);

    switch (TabID)
    {
    case 0:
        ImGui::SeparatorText("Visual");
        ImGui::Checkbox("ESP", &m_ESP);
        ImGui::Checkbox("Team ESP", &v_TeamESP);
        ImGui::Checkbox("Vehicle ESP", &v_VehicleESP);

        ImGui::NewLine();
        ImGui::SeparatorText("ESP Options");

        ImGui::Checkbox("Box", &v_Box);
        ImGui::Checkbox("Filled", &v_BoxFilled);
        ImGui::Checkbox("Line", &v_Line);
        ImGui::Checkbox("Skeleton", &v_Bone);
        ImGui::Checkbox("HealthBar", &v_Health);
        ImGui::Checkbox("Distance", &v_Distance);
        break;
    case 1:
        ImGui::SeparatorText("System");
        ImGui::Checkbox("StreamProof", &StreamProof);
        ImGui::Checkbox("AutoWindowFocus", &AutoWindowFocus);
        ImGui::Checkbox("VSync", &VSync);

        ImGui::NewLine();
        ImGui::SeparatorText("Game");
        ImGui::Checkbox("SpectatorList", &Spectator);
        ImGui::Checkbox("SwayEditor", &SwayEditor);
        if (SwayEditor)
            ImGui::SliderFloat("Sway", &SwayValue, 0.f, 1.f);
        ImGui::Checkbox("UnlockAll", &UnlockAll);
        break;
    case 2:
        ImGui::SeparatorText("Process");
        ImGui::Text("Process ID  : %d", m.PID);
        ImGui::Text("  pHandle   : 0x%lx\n", m.pHandle);

        ImGui::NewLine();
        ImGui::SeparatorText("Window");
        ImGui::Text(" ClassName  : %s", ClassName);
        ImGui::Text("WindowName  : %s", MenuName);
        ImGui::Text(" GameSize   : %d, %d", GameSize.right, GameSize.bottom);

        // Quit
        ImGui::NewLine();
        ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor(1.f, 1.f, 1.f, 0.35f));
        if (ImGui::Button("EXIT"))
            Run = false;
        ImGui::PopStyleColor();
        break;
    default:
        break;
    }

    ImGui::EndChild();
    // Right-Cild - End

    ImGui::SameLine();

    // Left-Cild
    ImGui::BeginChild("##LeftChild", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y), ImGuiWindowFlags_ChildMenu);

    switch (TabID)
    {
    case 0:
        ImGui::SeparatorText("ESP Colors");
        ImGui::ColorEdit4("Normal", &color_Normal.Value.x);
        ImGui::ColorEdit4("Visible", &color_Visible.Value.x);
        ImGui::ColorEdit4("Team", &color_Team.Value.x);
        ImGui::ColorEdit4("Skeleton", &color_bone.Value.x);

        ImGui::NewLine();
        ImGui::SeparatorText("ESP Configs");
        ImGui::Combo("Skeleton", &SklColorMode, SklList, IM_ARRAYSIZE(SklList));
        ImGui::SliderFloat("Distance", &MaxDistance, 25.f, 2000.f);
        break;
    case 1:
        break;
    case 2:
        ImGui::Text("Overlay : %.1fFPS", ImGui::GetIO().Framerate);
        break;
    default:
        break;
    }

    ImGui::EndChild();
    // Left-Cild - End

    ImGui::End();
}

Player player, * pEntity = &player;
Player local, * pLocal = &local;

// ToDo : Make a little better
void Core::ESP()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(GameSize.right, GameSize.bottom));
    ImGui::Begin("##BF4-ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    
    // Spectator NameList‚ðƒNƒŠƒA
    NameList.clear();

    // Context
    uint64_t ClientGameContext = m.RPM<uint64_t>(offset::ClientgameContext);
    uint64_t PlayerManager = m.RPM<uint64_t>(ClientGameContext + offset::PlayerManager);
    uint64_t PlayerEntity = m.RPM<uint64_t>(PlayerManager + offset::ClientPlayer);

    // LocalPlayer
    pLocal->ClientPlayer = m.RPM<uint64_t>(PlayerManager + offset::LocalPlayer);
    pLocal->Update();

    // ESP Loop
    for (int i = 0; i < 72; i++)
    {
        // LocalPlayer Check
        if (pLocal->IsDead() && !pLocal->InVehicle())
            break;

        // Update Player
        pEntity->ClientPlayer = m.RPM<uint64_t>(PlayerEntity + (i * 0x08));
        pEntity->Update();

        // Spectaror Warning
        if (pEntity->IsSpectator())
        {
            std::string spc_text = "[ Spectator found! ]";
            ImVec2 SpectextSize = ImGui::CalcTextSize(spc_text.c_str());

            char pName[16];
            ReadProcessMemory(m.pHandle, (void*)(pEntity->ClientPlayer + 0x1836), &pName, sizeof(pName), NULL);
            NameList.push_back(pName);

            String(ImVec2(GameSize.right / 2.f - (SpectextSize.x / 2.f), 0.f), ImColor(1.f, 0.f, 0.f, 1.f), spc_text.c_str());

            continue;
        }

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
        if (MaxDistance < distance)
            continue;

        // Vehicle ESP
        if (v_VehicleESP && pEntity->InVehicle())
        {
            if (!v_TeamESP && pEntity->Team == pLocal->Team)
                continue;

            DrawAABB(pEntity->VehicleAABB, pEntity->VehicleTranfsorm, pEntity->Team == pLocal->Team ? color_Team : color_Normal);

            if (v_Distance)
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
        else if (!v_TeamESP && pEntity->Team == pLocal->Team)
            continue;

        // WorldToScreen
        Vector2 ScreenPosition = Vector2(0.f, 0.f);
        if (!WorldToScreen(pEntity->Position, ScreenPosition))
            continue;

        // W2S Check
        if (ScreenPosition != Vector2(0.f, 0.f))
        {
            // Set ESP color
            ImColor color = pEntity->IsVisible() ? color_Visible : color_Normal;

            // Teammte
            if (v_TeamESP && pEntity->Team == pLocal->Team)
                color = color_Team;

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
            if (v_Box)
            {
                DrawBox(ScreenPosition.x - Width, ScreenPosition.y, Height / 2.f, -Height, color, ImColor(0.f, 0.f, 0.f, 0.4f));

                // Filled
                if (v_BoxFilled)
                    ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(BoxMiddle - Width, ScreenPosition.y), ImVec2(BoxMiddle + Width, ScreenPosition.y - Height), ImColor(0.f, 0.f, 0.f, 0.35f), NULL);
            }

            // Line
            if (v_Line)
                DrawLine(ImVec2(GameSize.right / 2, GameSize.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), color, 1);

            // Bone ESP
            if (v_Bone)
            {
                int aSkeleton[][2] =
                {
                { 104, 142 },{ 142, 9 },{ 9, 11 },{ 11, 15 },
                { 142,109},{109,111 },{111, 115},{ 142, 5 },
                { 5,  188},{ 5, 197},{ 188, 184},{ 197, 198},
                };

                for (int j = 0; j < 12; ++j)
                {
                    Vector3 Bone1 = GetBone(pEntity->ClientSoldier, aSkeleton[j][0]);
                    Vector3 Bone2 = GetBone(pEntity->ClientSoldier, aSkeleton[j][1]);

                    // Do you want bad performance?
                    if (Bone1 == Vector3(0.f, 0.f, 0.f) || Bone2 == Vector3(0.f, 0.f, 0.f))
                        break;

                    Vector2 Out1, Out2;
                    if (WorldToScreen(Bone1, Out1) && WorldToScreen(Bone2, Out2))
                    {
                        if (Out1.x > GameSize.right || Out1.x < GameSize.left)
                            break;

                        if (Out1 != Vector2(0.f, 0.f) || Out2 != Vector2(0.f, 0.f))
                        {
                            ImColor SKLColor = SklColorMode == 0 ? color : color_bone;

                            DrawLine(ImVec2(Out1.x, Out1.y), ImVec2(Out2.x, Out2.y), SKLColor, 1);
                        }
                           
                    }  
                }
            }

            // Health Bar
            if (v_Health)
                HealthBar(ScreenPosition.x - Width - 5, ScreenPosition.y + 1, 2.f, -Height - 1, pEntity->HealthBase->m_Health, pEntity->HealthBase->m_MaxHealth, true);

            // Distance
            if (v_Distance)
            {
                // float to Text
                std::string text = std::to_string((int)distance) + "m";
                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());

                String(ImVec2(ScreenPosition.x - (textSize.x / 2.f), ScreenPosition.y), ImColor(1.f, 1.f, 1.f, 1.f), text.c_str());
            }
        }

        ZeroMemory(pEntity, sizeof(pEntity));
    }

    ZeroMemory(pLocal, sizeof(pLocal));
    ImGui::End();
}

void Core::FuncMisc()
{
    // Recoil / Spread
    if (SwayEditor)
    {
        uint64_t Weapon = m.RPM<uint64_t>(offset::ClientWeapons);
        uint64_t weaponPtr = m.RPM<uint64_t>(Weapon + 0x128);
        uint64_t SwayData = m.RPM<uint64_t>(weaponPtr + 0x30);

        if (SwayData && m.RPM<float>(SwayData + 0x430) != SwayValue)
        {
            m.WPM<float>(SwayData + 0x430, SwayValue);
            m.WPM<float>(SwayData + 0x438, SwayValue);
            m.WPM<float>(SwayData + 0x434, SwayValue);
            m.WPM<float>(SwayData + 0x43C, SwayValue);
        }
    }
    else
    {
        uint64_t Weapon = m.RPM<uint64_t>(offset::ClientWeapons);
        uint64_t weaponPtr = m.RPM<uint64_t>(Weapon + 0x128);
        weaponPtr = m.RPM<uint64_t>(weaponPtr + 0x30);

        if (weaponPtr && m.RPM<float>(weaponPtr + 0x430) != 1.f)
        {
            m.WPM<float>(weaponPtr + 0x430, 1.f);
            m.WPM<float>(weaponPtr + 0x438, 1.f);
            m.WPM<float>(weaponPtr + 0x434, 1.f);
            m.WPM<float>(weaponPtr + 0x43C, 1.f);
        }
    }

    // UnlockAll
    if (UnlockAll)
    {
        uint64_t SyncBFSetting = m.RPM<uint64_t>(offset::SyncBFSetting);

        if (UnlockAll && m.RPM<bool>(SyncBFSetting + 0x54) == false)
            m.WPM<bool>(SyncBFSetting + 0x54, true);
    }
}

Vector3 Multiply(Vector3 vector, Matrix mat)
{
    return Vector3(mat._11 * vector.x + mat._21 * vector.y + mat._31 * vector.z,
        mat._12 * vector.x + mat._22 * vector.y + mat._32 * vector.z,
        mat._13 * vector.x + mat._23 * vector.y + mat._33 * vector.z);
}

void Core::DrawAABB(AxisAlignedBox aabb, Matrix tranform, ImColor color)
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