#ifndef NORA_DEFINES_H
#define NORA_DEFINES_H

// Unsigned integer types
typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;
typedef unsigned long long uint64;

// Signed integer types
typedef signed char int8;
typedef signed short int16;
typedef signed int int32;
typedef signed long long int64;

// Floating-point types
typedef float float32;
typedef double float64;

// Boolean types
typedef int bool32;
typedef char bool8;

// Define Release vs. Debug in Visual Studio
#ifdef _MSC_VER
    #ifdef _DEBUG
        #define NORA_DEBUG
    #else
        #define NORA_RELEASE
    #endif
#endif

// Define static assertions
#if defined(__clang__) || defined(__gcc__)
    #define STATIC_ASSERT _Static_assert
#else
    #define STATIC_ASSERT static_assert
#endif

// Ensure all above defined types are of the correct size
STATIC_ASSERT(sizeof(uint8) == 1, "Expected uint8 to be 1 byte.");
STATIC_ASSERT(sizeof(uint16) == 2, "Expected uint16 to be 2 bytes.");
STATIC_ASSERT(sizeof(uint32) == 4, "Expected uint32 to be 4 bytes.");
STATIC_ASSERT(sizeof(uint64) == 8, "Expected uint64 to be 8 bytes.");

STATIC_ASSERT(sizeof(int8) == 1, "Expected int8 to be 1 byte.");
STATIC_ASSERT(sizeof(int16) == 2, "Expected int16 to be 2 bytes.");
STATIC_ASSERT(sizeof(int32) == 4, "Expected int32 to be 4 bytes.");
STATIC_ASSERT(sizeof(int64) == 8, "Expected int64 to be 8 bytes.");

STATIC_ASSERT(sizeof(float32) == 4, "Expected float32 to be 4 bytes.");
STATIC_ASSERT(sizeof(float64) == 8, "Expected float64 to be 8 bytes.");

#define TRUE 1
#define FALSE 0

// Platform detection
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
    #define NORA_PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error "64-bit is required on Windows!"
    #endif
#elif defined(__linux__) || defined(__gnu_linux__)
    // Linux OS
    #define NORA_PLATFORM_LINUX 1
    #if defined(__ANDROID__)
        #define NORA_PLATFORM_ANDROID 1
    #endif
#elif defined(__unix__)
    // Catch anything not caught by the above.
    #define NORA_PLATFORM_UNIX 1
#elif defined(_POSIX_VERSION)
    // Posix
    #define NORA_PLATFORM_POSIX 1
#elif __APPLE__
    // Apple platforms
    #define NORA_PLATFORM_APPLE 1
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR
        // iOS Simulator
        #define NORA_PLATFORM_IOS 1
        #define NORA_PLATFORM_IOS_SIMULATOR 1
    #elif TARGET_OS_IPHONE
        #define NORA_PLATFORM_IOS 1
    // iOS device
    #elif TARGET_OS_MAC
    // Other kinds of Mac OS
    #else
        #error "Unknown Apple platform"
    #endif
#else
    #error "Unknown platform!"
#endif

#ifdef NORA_BUILD_DLL
    // Exports
    #ifdef _MSC_VER
        #define NORA_API __declspec(dllexport)
    #else
        #define NORA_API __attribute__((visibility("default")))
    #endif
#else
    // Imports
    #ifdef _MSC_VER
        #define NORA_API __declspec(dllimport)
    #else
        #define NORA_API
    #endif
#endif

#endif