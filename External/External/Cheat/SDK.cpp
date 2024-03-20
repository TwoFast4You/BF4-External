#include "SDK.h"

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
    uint64_t ragdoll_component = m.Read<uint64_t>(pSoldier + 0x580);
    if (!ragdoll_component)
        return Vector3(0, 0, 0);

    uint64_t quat = m.Read<uint64_t>(ragdoll_component + 0xB0);
    if (!quat)
        return Vector3(0, 0, 0);

    tmp = m.Read<Vector3>(quat + bone_id * 0x20);

    out.x = tmp.x;
    out.y = tmp.y;
    out.z = tmp.z;

    return out;
}

bool WorldToScreen(const Vector3& WorldPos, Vector2& ScreenPos)
{
    uint64_t GameRenderer = m.Read<uint64_t>(offset::GameRenderer);
    uint64_t RenderView = m.Read<uint64_t>(GameRenderer + 0x60);

    if (RenderView == 0)
        return false;

    uint64_t DXRenderer = m.Read<uint64_t>(offset::DxRenderer);
    uint64_t m_pScreen = m.Read<uint64_t>(DXRenderer + 0x38);

    if (m_pScreen == 0)
        return false;

    Matrix view_x_projection = m.Read<Matrix>(RenderView + 0x420);

    int ScreenWidth = m.Read<int>(m_pScreen + 0x58);
    int ScreenHeight = m.Read<int>(m_pScreen + 0x5C);

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
    uint64_t GameRenderer = m.Read<uint64_t>(offset::GameRenderer);
    uint64_t RenderView = m.Read<uint64_t>(GameRenderer + 0x60);

    if (RenderView == 0)
        return false;

    uint64_t DXRenderer = m.Read<uint64_t>(offset::DxRenderer);
    uint64_t m_pScreen = m.Read<uint64_t>(DXRenderer + 0x38);

    if (m_pScreen == 0)
        return false;

    Matrix view_x_projection = m.Read<Matrix>(RenderView + 0x420);

    int ScreenWidth = m.Read<int>(m_pScreen + 0x58);
    int ScreenHeight = m.Read<int>(m_pScreen + 0x5C);

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