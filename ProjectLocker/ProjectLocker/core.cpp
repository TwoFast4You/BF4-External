#include "core.h"
#include "Utils\NotSDK.h"

Vector3 GetBone(uint64_t pSoldier, int bone_id);
bool WorldToScreen(const Vector3& WorldPos, Vector2& ScreenPos);
bool WorldToScreen(const Vector3& WorldPos, Vector3& ScreenPos);
AxisAlignedBox GetAABB(uint64_t soldier);
float GetDistance(Vector3 value1, Vector3 value2);
Vector3 Multiply(Vector3 vector, Matrix mat);

extern bool Run;
extern bool AutoWindowFocus;
int TabID = 0;
const char* TabList[] = { "Visual", "Misc", "Developer" };
int v_BoxStyle = 0;
const char* BoxList[] = { "2D Box", "3D Box" };

void Cheat::Info()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(GameSize.right , GameSize.bottom));
    ImGui::Begin("##BF4-Info", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);

    ImGui::Text("Battlefield 4 External - % .1fFPS", ImGui::GetIO().Framerate);

    // Time
    time_t t = time(nullptr);
    struct tm nw;
    errno_t nTime = localtime_s(&nw, &t);
    ImGui::Text("%d:%d:%d", nw.tm_hour, nw.tm_min, nw.tm_sec);

    ImGui::End();
}

void Cheat::Menu()
{
    ImGui::SetNextWindowSize(ImVec2(600.f, 500.f));
    ImGui::Begin("Battlefield 4 [ EXTERNAL ]", (bool*)NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    float TabButtonSize = ImGui::GetContentRegionAvail().x;

    // Tab
    for (int i = 0; i < 3; i++)
    {
        if (ImGui::Button(TabList[i], ImVec2(TabButtonSize / 3.f - 8.f, 35.f)))
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
        ImGui::Checkbox("Line", &v_Line);
        ImGui::Checkbox("Bone", &v_Bone);
        ImGui::Checkbox("HealthBar", &v_Health);
        ImGui::Checkbox("Distance", &v_Distance);
        break;
    case 1:
        ImGui::NewLine();
        ImGui::SeparatorText("System");
        ImGui::Checkbox("AutoWindowFocus", &AutoWindowFocus);

        ImGui::NewLine();
        ImGui::SeparatorText("Game);
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
        ImGui::ColorEdit4("Normal", &color_Normal.x);
        ImGui::ColorEdit4("Visible", &color_Visible.x);
        ImGui::ColorEdit4("Team", &color_Team.x);

        ImGui::NewLine();
        ImGui::SeparatorText("ESP Configs");
        ImGui::SliderFloat("Distance", &MaxDistance, 25.f, 2000.f);
        ImGui::Combo("BoxStyle", &v_BoxStyle, BoxList, IM_ARRAYSIZE(BoxList));
        break;
    case 1:
        break;
    case 2:
        break;
    default:
        break;
    }

    ImGui::EndChild();
    // Left-Cild - End

    ImGui::End();
}

// ToDo : Make a little better
void Cheat::ESP()
{
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(GameSize.right, GameSize.bottom));
    ImGui::Begin("##BF4-ESP", (bool*)NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoInputs);
    
    // Context
    uint64_t ClientGameContext = m.RPM<uint64_t>(offset::ClientgameContext);
    uint64_t PlayerManager = m.RPM<uint64_t>(ClientGameContext + offset::PlayerManager);
    uint64_t PlayerEntity = m.RPM<uint64_t>(PlayerManager + offset::ClientPlayer);

    // Local
    uint64_t LocalPlayer = m.RPM<uint64_t>(PlayerManager + offset::LocalPlayer);
    uint64_t LocalSoldier = m.RPM<uint64_t>(LocalPlayer + offset::ClientSoldier);
    uint64_t LocalVehicle = m.RPM<uint64_t>(PlayerManager + offset::ClientVehicle);
    int LocalTeam = m.RPM<int>(LocalPlayer + offset::PlayerTeam);
    uint64_t LHealthComponent = m.RPM<uint64_t>(LocalSoldier + 0x140);
    float LocalHealth = m.RPM<float>(LHealthComponent + 0x20);
    uint64_t LPosComponent = m.RPM<uint64_t>(LocalSoldier + 0x490);
    Vector3 LocalPosition = m.RPM<Vector3>(LPosComponent + 0x30);

    // LocalPlayer valid check
    if (LocalHealth <= 0.f && LocalPosition == Vector3(0.f, 0.f, 0.f) && !LocalVehicle)
        Alive = false;
    else
        Alive = true;

    // ESP Loop
    for (int i = 0; i < 70; i++)
    {
        if (!Alive)
            continue;

        uint64_t ClientPlayer = m.RPM<uint64_t>(PlayerEntity + (i * 0x08));
        uint64_t ClientSoldier = m.RPM<uint64_t>(ClientPlayer + offset::ClientSoldier);
        uint64_t VehicleEntity = m.RPM<uint64_t>(ClientPlayer + 0x14C0);

        // Vehicle Context 
        AxisAlignedBox VehicleAABB;
        Matrix VehicleTranfsorm;

        // Invalid Player
        if (ClientPlayer == NULL)
            continue;
        else if (ClientPlayer == LocalPlayer)
            continue;
        else if (VehicleEntity == LocalVehicle && LocalVehicle != 0 && VehicleEntity != 0)
            continue;

        // Team
        int Team = m.RPM<int>(ClientPlayer + offset::PlayerTeam);

        // in Vehicle
        if (VehicleEntity)
        {
            if (!v_VehicleESP)
                continue;

            uint64_t pDynamicPhysicsEntity = m.RPM<uint64_t>(VehicleEntity + 0x238);

            if (pDynamicPhysicsEntity)
            {
                uint64_t pPhysicsEntity = m.RPM<uint64_t>(pDynamicPhysicsEntity + 0xA0); // EntityTransform
                VehicleTranfsorm = m.RPM<Matrix>(pPhysicsEntity + 0x0); // Transform
                VehicleAABB = m.RPM<AxisAlignedBox>(VehicleEntity + 0x250);

                ImColor v_color = color_Normal;

                if (v_TeamESP && LocalTeam == Team)
                    v_color = color_Team;
                else if (!v_TeamESP && LocalTeam == Team)
                    continue;

                DrawAABB(VehicleAABB, VehicleTranfsorm, v_color);
            }
        }

        // Spectaror warning - tmp
        bool spect = m.RPM<bool>(ClientPlayer + offset::Spectator);
        if (spect)
        {
            std::string spc_text = "Spectator found!";
            ImVec2 SpectextSize = ImGui::CalcTextSize(spc_text.c_str());
            float SpecTextCentor = SpectextSize.x / 2.f;
            String(ImVec2(GameSize.right / 2.f - SpecTextCentor, GameSize.bottom), ImColor(1.f, 0.f, 0.f, 1.f), spc_text.c_str());
        }

        // Health
        uint64_t HealthComponent = m.RPM<uint64_t>(ClientSoldier + 0x140);
        float Health = m.RPM<float>(HealthComponent + 0x20);

        // Position
        uint64_t PositionComponent = m.RPM<uint64_t>(ClientSoldier + 0x490);
        Vector3 PlayerPosition = m.RPM<Vector3>(PositionComponent + 0x30);
        float distance = GetDistance(LocalPosition, PlayerPosition);

        // Check ESP MaxDistance
        if (MaxDistance < distance)
            continue;

        // Visible
        bool Visible = m.RPM<bool>(ClientSoldier + 0x5B1);
        Visible = !Visible; // Need it

        // Some check
        if (Health <= 0.f)
            continue;
        else if (Team == LocalTeam && !v_TeamESP)
            continue;
        else if (PlayerPosition == Vector3(0.f, 0.f, 0.f))
            continue;

        // WorldToScreen
        Vector2 ScreenPosition = {};
        WorldToScreen(PlayerPosition, ScreenPosition);

        // W2S Check
        if (ScreenPosition != Vector2(0.f, 0.f))
        {
            // Set ESP color
            ImColor color = Visible ? color_Visible : color_Normal;

            // Teammte
            if (v_TeamESP && Team == LocalTeam)
                color = color_Team;

            // Get some size.
            Vector3 Top = PlayerPosition + GetAABB(ClientSoldier).Max;
            Vector3 Btm = PlayerPosition + GetAABB(ClientSoldier).Min;
            Vector2 BoxTop, BoxBtm;
            WorldToScreen(Top, BoxTop);
            WorldToScreen(Btm, BoxBtm);

            float BoxMiddle = ScreenPosition.x;
            float Height = BoxBtm.y - BoxTop.y;
            float Width = Height / 4.f;

            // Box
            if (v_Box)
            {
                float Yaw = m.RPM<float>(ClientSoldier + 0x4D8);

                switch (v_BoxStyle)
                {
                case 0: // 2D
                    Line(ImVec2(ScreenPosition.x - Width, ScreenPosition.y), ImVec2(ScreenPosition.x + Width, ScreenPosition.y), color, 1);
                    Line(ImVec2(ScreenPosition.x - Width, ScreenPosition.y - Height), ImVec2(ScreenPosition.x + Width, ScreenPosition.y - Height), color, 1);
                    Line(ImVec2(ScreenPosition.x - Width, ScreenPosition.y), ImVec2(ScreenPosition.x - Width, ScreenPosition.y - Height), color, 1);
                    Line(ImVec2(ScreenPosition.x + Width, ScreenPosition.y), ImVec2(ScreenPosition.x + Width, ScreenPosition.y - Height), color, 1);
                    break;
                case 1: // 3D
                    DrawBox(GetAABB(ClientSoldier), PlayerPosition, Yaw, color);
                    break;
                default:
                    break;
                }
            }

            // Line
            if (v_Line)
                Line(ImVec2(GameSize.right / 2, GameSize.bottom), ImVec2(ScreenPosition.x, ScreenPosition.y), color, 1);

            // Bone ESP
            if (v_Bone)
            {
                int aSkeleton[][2] =
                {
                { 104, 142 },{ 142, 9 },{ 9, 11 },{ 11, 15 },
                { 142,109},{109,111 },{111, 115},{ 142, 5 },
                { 5,  188},{ 5, 197},{ 188, 184},{ 197, 198},
                };

                for (int i = 0; i < 12; ++i)
                {
                    Vector3 Bone1 = GetBone(ClientSoldier, aSkeleton[i][0]);
                    Vector3 Bone2 = GetBone(ClientSoldier, aSkeleton[i][1]);

                    // Do you want bad performance?
                    if (Bone1 == Vector3(0.f, 0.f, 0.f) || Bone2 == Vector3(0.f, 0.f, 0.f))
                        continue;

                    Vector3 Out1, Out2, Out3;
                    if (WorldToScreen(Bone1, Out1) && WorldToScreen(Bone2, Out2))
                        Line(ImVec2(Out1.x, Out1.y), ImVec2(Out2.x, Out2.y), ImColor(1.f, 1.f, 1.f, 1.f), 1.f);
                }
            }

            // Health Bar
            if (v_Health)
            {
                BaseBar(ScreenPosition.x - Width - 5, ScreenPosition.y + 1, 3, -Height - 1, 100);
                ProgressBar(ScreenPosition.x - Width - 4, ScreenPosition.y, 1, -Height, Health, 100);
            }

            // Distance
            if (v_Distance)
            {
                // float to Text
                std::string text = std::to_string((int)distance) + "m";

                // Magic
                ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
                float TextCentor = textSize.x / 2.f;

                // Render
                String(ImVec2(ScreenPosition.x - TextCentor, ScreenPosition.y), ImColor(1.f, 1.f, 1.f, 1.f), text.c_str());
            }
        }
    }

    ImGui::End();
}

void Cheat::FuncMisc()
{
    // Recoil / Spread
    if (SwayEditor)
    {
        uint64_t Weapon = m.RPM<uint64_t>(offset::ClientWeapons);

        // Sway
        uint64_t weaponPtr = m.RPM<uint64_t>(Weapon + 0x128);
        weaponPtr = m.RPM<uint64_t>(weaponPtr + 0x30);

        if (weaponPtr && m.RPM<float>(weaponPtr + 0x430) != SwayValue && Alive)
        {
            m.WPM<float>(weaponPtr + 0x430, SwayValue);
            m.WPM<float>(weaponPtr + 0x438, SwayValue);
            m.WPM<float>(weaponPtr + 0x434, SwayValue);
            m.WPM<float>(weaponPtr + 0x43C, SwayValue);
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

void Cheat::DrawBox(AxisAlignedBox aabb, Vector3 m_Position, float Yaw, ImColor color)
{
    float cosY = (float)cos(Yaw);
    float sinY = (float)sin(Yaw);

    Vector3 fld = Vector3(aabb.Min.z * cosY - aabb.Min.x * sinY, aabb.Min.y, aabb.Min.x * cosY + aabb.Min.z * sinY) + m_Position; // 0
    Vector3 brt = Vector3(aabb.Min.z * cosY - aabb.Max.x * sinY, aabb.Min.y, aabb.Max.x * cosY + aabb.Min.z * sinY) + m_Position; // 1
    Vector3 bld = Vector3(aabb.Max.z * cosY - aabb.Max.x * sinY, aabb.Min.y, aabb.Max.x * cosY + aabb.Max.z * sinY) + m_Position; // 2
    Vector3 frt = Vector3(aabb.Max.z * cosY - aabb.Min.x * sinY, aabb.Min.y, aabb.Min.x * cosY + aabb.Max.z * sinY) + m_Position; // 3
    Vector3 frd = Vector3(aabb.Max.z * cosY - aabb.Min.x * sinY, aabb.Max.y, aabb.Min.x * cosY + aabb.Max.z * sinY) + m_Position; // 4
    Vector3 brb = Vector3(aabb.Min.z * cosY - aabb.Min.x * sinY, aabb.Max.y, aabb.Min.x * cosY + aabb.Min.z * sinY) + m_Position; // 5
    Vector3 blt = Vector3(aabb.Min.z * cosY - aabb.Max.x * sinY, aabb.Max.y, aabb.Max.x * cosY + aabb.Min.z * sinY) + m_Position; // 6
    Vector3 flt = Vector3(aabb.Max.z * cosY - aabb.Max.x * sinY, aabb.Max.y, aabb.Max.x * cosY + aabb.Max.z * sinY) + m_Position; // 7

    if (!WorldToScreen(fld, fld) || !WorldToScreen(brt, brt)
        || !WorldToScreen(bld, bld) || !WorldToScreen(frt, frt)
        || !WorldToScreen(frd, frd) || !WorldToScreen(brb, brb)
        || !WorldToScreen(blt, blt) || !WorldToScreen(flt, flt))
        return;

    Line(ImVec2(fld.x, fld.y), ImVec2(brt.x, brt.y), color, 1.f);
    Line(ImVec2(brb.x, brb.y), ImVec2(blt.x, blt.y), color, 1.f);
    Line(ImVec2(fld.x, fld.y), ImVec2(brb.x, brb.y), color, 1.f);
    Line(ImVec2(brt.x, brt.y), ImVec2(blt.x, blt.y), color, 1.f);
    Line(ImVec2(frt.x, frt.y), ImVec2(bld.x, bld.y), color, 1.f);
    Line(ImVec2(frd.x, frd.y), ImVec2(flt.x, flt.y), color, 1.f);
    Line(ImVec2(frt.x, frt.y), ImVec2(frd.x, frd.y), color, 1.f);
    Line(ImVec2(bld.x, bld.y), ImVec2(flt.x, flt.y), color, 1.f);
    Line(ImVec2(frt.x, frt.y), ImVec2(fld.x, fld.y), color, 1.f);
    Line(ImVec2(frd.x, frd.y), ImVec2(brb.x, brb.y), color, 1.f);
    Line(ImVec2(brt.x, brt.y), ImVec2(bld.x, bld.y), color, 1.f);
    Line(ImVec2(blt.x, blt.y), ImVec2(flt.x, flt.y), color, 1.f);
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

    DrawLineEx(fld, flt, color);
    DrawLineEx(flt, frt, color);
    DrawLineEx(frt, frd, color);
    DrawLineEx(frd, fld, color);
    DrawLineEx(bld, blt, color);
    DrawLineEx(blt, brt, color);
    DrawLineEx(brt, brb, color);
    DrawLineEx(brb, bld, color);
    DrawLineEx(fld, bld, color);
    DrawLineEx(frd, brb, color);
    DrawLineEx(flt, blt, color);
    DrawLineEx(frt, brt, color);
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

AxisAlignedBox GetAABB(uint64_t soldier)
{
    AxisAlignedBox aabb = {};
    int Pose = m.RPM<int>(soldier + 0x4F0);

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

float GetDistance(Vector3 value1, Vector3 value2)
{
    float num = value1.x - value2.x;
    float num2 = value1.y - value2.y;
    float num3 = value1.z - value2.z;

    return sqrt(num * num + num2 * num2 + num3 * num3);
}

Vector3 Multiply(Vector3 vector, Matrix mat)
{
    return Vector3(mat._11 * vector.x + mat._21 * vector.y + mat._31 * vector.z,
        mat._12 * vector.x + mat._22 * vector.y + mat._32 * vector.z,
        mat._13 * vector.x + mat._23 * vector.y + mat._33 * vector.z);
}
