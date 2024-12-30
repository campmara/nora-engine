#include "nora_app_windows.h"
#include "rendering/nora_renderer_d3d11.h"

namespace nora
{
    void ApplicationWindows::Initialize()
    {
        hinstance = GetModuleHandle(NULL);

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = hinstance;
        wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = NULL;
        wc.lpszClassName = APP_NAME;
        wc.hIconSm = wc.hIcon;

        RegisterClassEx(&wc);

        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
        int32 posX, posY;

        if (isFullScreen)
        {
            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            dmScreenSettings.dmBitsPerPel = 32;
            dmScreenSettings.dmPelsWidth = (DWORD)screenWidth;
            dmScreenSettings.dmPelsHeight = (DWORD)screenHeight;

            ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

            posX = posY = 0;
        }
        else
        {
            screenWidth = DEFAULT_WINDOW_WIDTH;
            screenHeight = DEFAULT_WINDOW_HEIGHT;
            posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
            posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
        }

        hwnd = CreateWindowEx(WS_EX_APPWINDOW, APP_NAME, APP_NAME,
                              WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP, posX, posY, screenWidth,
                              screenHeight, NULL, NULL, hinstance, NULL);

        ShowWindow(hwnd, SW_SHOW);
        SetForegroundWindow(hwnd);
        SetFocus(hwnd);

        ShowCursor(true);

        // Set up the d3d11 renderer for now.
        renderer = new RendererD3D11(&hwnd);
        renderer->Initialize();
    }

    void ApplicationWindows::MainLoop()
    {
        while (true)
        {
            renderer->ProcessFrame();
        }
    }

    void ApplicationWindows::Shutdown()
    {
        renderer->Shutdown();
        delete renderer;
        renderer = nullptr;

        ShowCursor(true);

        if (isFullScreen)
        {
            ChangeDisplaySettings(NULL, 0);
        }

        DestroyWindow(hwnd);
        hwnd = NULL;

        UnregisterClass(APP_NAME, hinstance);
        hinstance = NULL;
    }

    HWND ApplicationWindows::GetWindowHandle() const
    {
        return hwnd;
    }
} // namespace nora