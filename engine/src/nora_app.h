#ifndef NORA_APP_H
#define NORA_APP_H

#include "nora.h"

struct NORA_API nora_app
{
    virtual void Init() = 0;
    virtual void Run() = 0;
    virtual void Close() = 0;
};

#endif