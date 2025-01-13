#ifndef NORA_APP_WINDOWS_H
#define NORA_APP_WINDOWS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "nora_app.h"
#include "rendering/nora_renderer_d3d11.h"

namespace nora
{
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

#pragma region WndProc Callback Handler

        static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
        {
            LRESULT result = 0;

            switch (uMessage)
            {
                case WM_DESTROY:
                {
                    PostQuitMessage(0);
                    break;
                }
                case WM_CLOSE:
                {
                    PostQuitMessage(0);
                    break;
                }
                case WM_KEYDOWN:
                {
                    if (wParam == VK_ESCAPE)
                    {
                        PostMessage(hwnd, WM_CLOSE, 0, 0);
                    }
                    break;
                }
                default:
                {
                    result = DefWindowProc(hwnd, uMessage, wParam, lParam);
                    break;
                }
            }

            return result;
        }

#pragma endregion

    private:
        const LPCWSTR APP_NAME = L"Lake of Mirrors";
        HINSTANCE instance_handle = NULL;
        HWND window_handle = NULL;

        RendererD3D11 *renderer = nullptr;
    };
} // namespace nora

#endif