#pragma once

// Os detection
#if defined(_WIN32) || defined(_WIN64)
#define CUBIC_NBT_OS_WINDOWS
#elif defined(__unix__) && defined(__linux__)
#define CUBIC_NBT_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define CUBIC_NBT_OS_MACOS
#else
#error "Unsupported operating system"
#endif

// Export and import macros
#if defined(CUBIC_NBT_OS_WINDOWS)
#define CUBIC_NBT_EXPORT __declspec(dllexport)
#define CUBIC_NBT_IMPORT __declspec(dllimport)
#else
#define CUBIC_NBT_EXPORT __attribute__((visibility("default")))
#define CUBIC_NBT_IMPORT __attribute__((visibility("default")))
#endif

#if defined(CUBIC_NBT_BUILD)
#define CUBIC_NBT_API CUBIC_NBT_EXPORT
#else
#define CUBIC_NBT_API CUBIC_NBT_IMPORT
#endif

// Endianess macros
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BIG_ENDIAN__)            \
    || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) \
    || defined(__MIBSEB) || defined(__MIBSEB__)
// It's a big-endian target architecture
#define CUBIC_NBT_ENDIAN_BIG
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__)    \
    || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) \
    || defined(__MIPSEL) || defined(__MIPSEL__)
// It's a little-endian target architecture
#define CUBIC_NBT_ENDIAN_LITTLE
#else
#define CUBIC_NBT_ENDIAN_UNKNOWN
#endif
