#ifndef NORA_APP_WINDOWS_H
#define NORA_APP_WINDOWS_H

#include "nora_app.h"

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
    default:
        return DefWindowProc(hwnd, uMessage, wParam, lParam);
    }
}

#pragma endregion

struct NORA_API nora_app_windows : public nora_app
{
    void Init();
    void Run();
    void Close();

private:
    const LPCWSTR APP_NAME = L"Nora and the Lake of Mirrors";
    HINSTANCE hinstance = NULL;
    HWND hwnd = NULL;
};

#endif