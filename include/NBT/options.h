#pragma once

// Os detection
#if defined(_WIN32) || defined(_WIN64)
#define NBT_OS_WINDOWS
#elif defined(__unix__) && defined(__linux__)
#define NBT_OS_LINUX
#elif defined(__APPLE__) && defined(__MACH__)
#define NBT_OS_MACOS
#else
#error "Unsupported operating system"
#endif

// Export and import macros
#if defined(NBT_OS_WINDOWS)
#define NBT_EXPORT __declspec(dllexport)
#define NBT_IMPORT __declspec(dllimport)
#else
#define NBT_EXPORT __attribute__((visibility("default")))
#define NBT_IMPORT __attribute__((visibility("default")))
#endif

#if defined(NBT_BUILD)
#define NBT_API NBT_EXPORT
#else
#define NBT_API NBT_IMPORT
#endif

// Exception macros
// #ifdef NBT_NO_THROW
// #define NBT_THROW(exception)
// #else
// #define NBT_THROW(exception) throw exception
// #endif

// Wrapping macros
#ifdef NBT_CUBIC_WRAP
#define CUBIC_WRAP_BEGIN namespace cubic {
#define CUBIC_WRAP_END }
#define CUBIC_WRAP_USING using namespace cubic
#define CUBIC_WRAP ::cubic
#else
#define CUBIC_WRAP_BEGIN
#define CUBIC_WRAP_END
#define CUBIC_WRAP_USING
#define CUBIC_WRAP
#endif

// Endianess macros
#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || defined(__BIG_ENDIAN__)            \
    || defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || defined(_MIBSEB) \
    || defined(__MIBSEB) || defined(__MIBSEB__)
// It's a big-endian target architecture
#define NBT_ENDIAN_BIG
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || defined(__LITTLE_ENDIAN__)    \
    || defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || defined(_MIPSEL) \
    || defined(__MIPSEL) || defined(__MIPSEL__)
// It's a little-endian target architecture
#define NBT_ENDIAN_LITTLE
#else
#define NBT_ENDIAN_UNKNOWN
#endif

// Attribute macros
#define UNUSED __attribute__((unused))
#define NODISCARD [[nodiscard]]
