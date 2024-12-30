#ifndef NORA_APP_H
#define NORA_APP_H

#include "nora_defines.h"

namespace nora
{
    struct NORA_API ApplicationBase
    {
        ApplicationBase() = default;
        virtual ~ApplicationBase() {}
        ApplicationBase(const ApplicationBase &) = delete;
        ApplicationBase &operator=(const ApplicationBase &) = delete;

        virtual void Initialize() = 0;
        virtual void MainLoop() = 0;
        virtual void Shutdown() = 0;

        const uint32 DEFAULT_WINDOW_WIDTH = 1366;
        const uint32 DEFAULT_WINDOW_HEIGHT = 768;

        uint32 screenWidth = 0;
        uint32 screenHeight = 0;

        uint32 windowWidth = 0;
        uint32 windowHeight = 0;

        bool32 isFullScreen = false;
    };
} // namespace nora

#endif