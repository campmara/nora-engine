#ifndef NORA_APP_WINDOWS_H
#define NORA_APP_WINDOWS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "nora_app.h"
#include "rendering/nora_renderer_d3d11.h"

namespace nora
{
#pragma region WndProc Callback Handler

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
    {
        switch (uMessage)
        {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE)
            {
                PostMessage(hwnd, WM_CLOSE, 0, 0);
            }
            return 0;
        default:
            return DefWindowProc(hwnd, uMessage, wParam, lParam);
        }
    }

#pragma endregion

    struct NORA_API ApplicationWindows final : public ApplicationBase
    {
        ApplicationWindows() {}
        virtual ~ApplicationWindows() {}
        ApplicationWindows(const ApplicationWindows &) = delete;
        ApplicationWindows &operator=(const ApplicationWindows &) = delete;

        void Initialize();
        void MainLoop();
        void Shutdown();

        HWND GetWindowHandle() const;

    private:
        const LPCWSTR APP_NAME = L"Nora and the Lake of Mirrors";
        HINSTANCE hinstance = NULL;
        HWND hwnd = NULL;

        RendererD3D11 *renderer = nullptr;
    };
}

#endif