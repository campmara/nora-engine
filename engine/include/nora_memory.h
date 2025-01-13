#ifndef NORA_MEMORY_H
#define NORA_MEMORY_H

#include "nora_defines.h"

#define KILOBYTES(value) ((value) * 1024)
#define MEGABYTES(value) KILOBYTES(value) * 1024
#define GIGABYTES(value) MEGABYTES(value) * 1024

namespace nora
{
    struct MemoryArena
    {
        bool32 is_initialized;
        uint64 permanent_storage_size;
        void *permanent_storage;
    };
}

#endif // !NORA_MEMORY_H
