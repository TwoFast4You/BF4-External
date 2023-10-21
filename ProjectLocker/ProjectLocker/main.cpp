#include "core.h"

Cheat c, * Ct = &c;

HWND OverlayHwnd;
bool Run = true;
bool ShowMenu = true;
bool AutoWindowFocus = true;
int MenuKey = VK_INSERT;

bool IsKeyDown(int VK);
void CThread();

//int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hInstPrev, PSTR cmdline, int cmdshow)
int main()
{
    // Fix DIP Scale
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_SYSTEM_AWARE);

    // Init Memory
    if (!m.Init())
        return -1;

    // Get window info
    GetClassNameA(m.GameHwnd, Ct->ClassName, sizeof(Ct->ClassName));
    GetWindowTextA(m.GameHwnd, Ct->MenuName, sizeof(Ct->MenuName));
    GetClientRect(m.GameHwnd, &Ct->GameSize);
    ClientToScreen(m.GameHwnd, &Ct->GamePOINT);

    // Create Overlay - TopMost‚Å‚ ‚é•K—v‚Í‚È‚¢
    WNDCLASSEXA wc = { sizeof(WNDCLASSEXA), 0, WndProc, 0, 0, NULL, NULL, NULL, NULL, "NULL", "Wip", NULL };
    RegisterClassExA(&wc);
    OverlayHwnd = CreateWindowExA(WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOPMOST | WS_EX_TOOLWINDOW, wc.lpszClassName, wc.lpszMenuName, WS_POPUP, Ct->GamePOINT.x, Ct->GamePOINT.y, Ct->GameSize.right, Ct->GameSize.bottom, NULL, NULL, wc.hInstance, NULL);
    SetLayeredWindowAttributes(OverlayHwnd, RGB(0, 0, 0), 255, LWA_ALPHA);
    MARGINS margin = { -1 };
    DwmExtendFrameIntoClientArea(OverlayHwnd, &margin);

    if (!CreateDeviceD3D(OverlayHwnd))
    {
        CleanupDeviceD3D();
        UnregisterClassA(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    ShowWindow(OverlayHwnd, SW_SHOWDEFAULT);
    UpdateWindow(OverlayHwnd);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    // Load ImGui Style.
    ImStyle();

    ImGui_ImplWin32_Init(OverlayHwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    ImVec4 clear_color = ImVec4(1.0f, 1.0f, 1.0f, 0.f);

    // Create Cheat thread.
    HANDLE MainThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)CThread, NULL, 0, NULL);
    if (MainThread)
        CloseHandle(MainThread);

    // Main loop
    while (Run)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                Run = false;
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        /*--| ESP / Menu Core. |-------------------------------*/

        Ct->Info();

        if (ShowMenu)
            Ct->Menu();

        if (Ct->m_ESP)
            Ct->ESP();

        /*-----------------------------------------------------*/

        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0);
    }

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(OverlayHwnd);
    UnregisterClassA(wc.lpszClassName, wc.hInstance);

    return 0;
}

void CThread()
{
    // Set window style data
    static LONG MenuStyle = WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW;
    static LONG ESPStyle = WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW;

    // Thread Main loop
    while (Run)
    {
        // Toggle Menu - Default menu key : Insert
        static bool tmp = false;
        if (IsKeyDown(MenuKey) && !tmp)
        {
            ShowMenu = !ShowMenu;
            tmp = true;
        }
        else if (!IsKeyDown(MenuKey) && tmp)
        {
            tmp = false;
        }

        //----// Window Style Changer //---------------------------------------------------------------//
        // Get window style.
        LONG TmpWindowLong = GetWindowLong(OverlayHwnd, GWL_EXSTYLE);

        // Check window style.
        if (ShowMenu && TmpWindowLong != MenuStyle)
            SetWindowLong(OverlayHwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
        else if (!ShowMenu && TmpWindowLong != ESPStyle)
            SetWindowLong(OverlayHwnd, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

        //---------------------------------------------------------------------------------------------//

        // Overlay size and position changer.
        HWND TmpHwnd = FindWindowA(NULL, "Battlefield 4");
        if (TmpHwnd)
        {
            // Magic
            if (AutoWindowFocus)
            {
                if (ShowMenu && GetForegroundWindow() == TmpHwnd)
                    SetForegroundWindow(OverlayHwnd);
                else if (!ShowMenu && GetForegroundWindow() == OverlayHwnd)
                    SetForegroundWindow(TmpHwnd);
            }

            RECT TmpRect = {};
            POINT TmpPoint = {};
            GetClientRect(TmpHwnd, &TmpRect);
            ClientToScreen(TmpHwnd, &TmpPoint);

            if (Ct->GamePOINT.x != TmpPoint.x || Ct->GamePOINT.x != TmpPoint.y || Ct->GameSize.left != TmpRect.left || Ct->GameSize.bottom != TmpRect.bottom || Ct->GameSize.top != TmpRect.top || Ct->GameSize.right != TmpRect.right)
            {
                Ct->GameSize = TmpRect;
                Ct->GamePOINT = TmpPoint;

                SetWindowPos(OverlayHwnd, NULL, TmpPoint.x, TmpPoint.y, TmpRect.right, TmpRect.bottom, SWP_NOREDRAW);
            }
        }
        else
        {
            break;
        }

        // Run Misc function from Cheat Class.
        Ct->FuncMisc();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    Run = false;
    ExitThread(0);
}

bool IsKeyDown(int VK)
{
    return (GetAsyncKeyState(VK) & 0x8000) != 0;
}
