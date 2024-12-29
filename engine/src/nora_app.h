#ifndef NORA_APP_H
#define NORA_APP_H

#include "nora.h"

struct NORA_API nora_app
{
public:
    virtual void Init() = 0;
    virtual void Run() = 0;
    virtual void Close() = 0;

    const uint32 DEFAULT_WINDOW_WIDTH = 1366;
    const uint32 DEFAULT_WINDOW_HEIGHT = 768;

    uint32 screenWidth = 0;
    uint32 screenHeight = 0;

    uint32 windowWidth = 0;
    uint32 windowHeight = 0;

    bool32 isFullScreen = false;
};

#endif