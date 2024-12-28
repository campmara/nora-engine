#ifndef NORA_H
#define NORA_H

#include "nora_defines.h"

#ifdef NORA_PLATFORM_WINDOWS
    // Directives
    #define WIN32_LEAN_AND_MEAN

    // System Includes
    #include <windows.h>

    // Engine Includes
    #include "nora_app_windows.h"
#endif

#endif