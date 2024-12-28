#ifndef NORA_APP_WINDOWS_H
#define NORA_APP_WINDOWS_H

#include "nora_app.h"

struct NORA_API nora_app_windows : public nora_app
{
    void Init() {}

    void Run()
    {
        while (true)
        {
        }
    }

    void Close() {}
};

#endif