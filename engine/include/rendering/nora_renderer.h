#ifndef NORA_RENDERER_H
#define NORA_RENDERER_H

#include "nora_defines.h"

namespace nora
{
    struct RendererBase
    {
        RendererBase() = default;
        virtual ~RendererBase() {}
        RendererBase(const RendererBase &) = delete;
        RendererBase &operator=(const RendererBase &) = delete;

        virtual void Initialize() = 0;
        virtual void ProcessFrame() = 0;
        virtual void Shutdown() = 0;
    };
} // namespace nora

#endif