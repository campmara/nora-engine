#include "application/nora_app_windows.h"
#include "rendering/nora_renderer_d3d11.h"

namespace nora
{
    void ApplicationWindows::Initialize()
    {
        instance_handle = GetModuleHandle(NULL);

        WNDCLASSEX wc = {};
        wc.cbSize = sizeof(WNDCLASSEX);
        wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wc.lpfnWndProc = WndProc;
        wc.cbClsExtra = 0;
        wc.cbWndExtra = 0;
        wc.hInstance = instance_handle;
        wc.hIcon = LoadIcon(0, IDI_APPLICATION);
        wc.hCursor = LoadCursor(0, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
        wc.lpszMenuName = 0;
        wc.lpszClassName = APP_NAME;
        wc.hIconSm = wc.hIcon;

        RegisterClassEx(&wc);

        screen_width = GetSystemMetrics(SM_CXSCREEN);
        screen_height = GetSystemMetrics(SM_CYSCREEN);
        int32 pos_x, pos_y;

        if (is_fullscreen)
        {
            DEVMODE dmScreenSettings;
            memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
            dmScreenSettings.dmSize = sizeof(dmScreenSettings);
            dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            dmScreenSettings.dmBitsPerPel = 32;
            dmScreenSettings.dmPelsWidth = (DWORD)screen_width;
            dmScreenSettings.dmPelsHeight = (DWORD)screen_height;

            ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

            pos_x = pos_y = 0;
        }
        else
        {
            screen_width = DEFAULT_WINDOW_WIDTH;
            screen_height = DEFAULT_WINDOW_HEIGHT;
            pos_x = (GetSystemMetrics(SM_CXSCREEN) - screen_width) / 2;
            pos_y = (GetSystemMetrics(SM_CYSCREEN) - screen_height) / 2;
        }

        window_handle = CreateWindowExW(WS_EX_APPWINDOW, APP_NAME, APP_NAME,
                                       WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_OVERLAPPEDWINDOW, pos_x, pos_y,
                                       screen_width, screen_height, NULL, NULL, instance_handle, NULL);

        ShowWindow(window_handle, SW_SHOW);
        SetForegroundWindow(window_handle);
        SetFocus(window_handle);

        ShowCursor(true);

        // Set up the d3d11 renderer for now.
        renderer = new RendererD3D11(&window_handle);
        renderer->Initialize();
    }

    void ApplicationWindows::MainLoop()
    {
        if (!window_handle)
        {
            return;
        }

        while (true)
        {
            MSG message;

            // Windows Message Handling
            if (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&message);
                DispatchMessage(&message);

                if (message.message == WM_QUIT)
                {
                    break;
                }
            }
            else
            {
                // Main Game Loop
                renderer->ProcessFrame();
            }
        }
    }

    void ApplicationWindows::Shutdown()
    {
        renderer->Shutdown();
        delete renderer;
        renderer = nullptr;

        ShowCursor(true);

        if (is_fullscreen)
        {
            ChangeDisplaySettings(NULL, 0);
        }

        DestroyWindow(window_handle);
        window_handle = NULL;

        UnregisterClass(APP_NAME, instance_handle);
        instance_handle = NULL;
    }

    HWND ApplicationWindows::GetWindowHandle() const
    {
        return window_handle;
    }
} // namespace nora