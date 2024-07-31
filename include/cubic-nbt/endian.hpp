#pragma once

#include <cubic-nbt/options.h>

#include <cstdint>
#include <endian.h>
#include <type_traits>

#ifndef CUBIC_NBT_ENDIAN_BIG

namespace cubic::nbt::details {

template<typename T>
CUBIC_NBT_API auto byte_swap(T value) -> T
{
    static_assert(std::is_trivial<T>::value, "Type must be trivial");
    static_assert(std::is_arithmetic<T>::value, "Type must be arithmetic");
    static_assert(sizeof(T) <= sizeof(uint64_t), "Type must be less than or equal to 64 bits");

    T result = 0;
    auto *src = reinterpret_cast<uint8_t *>(&value);
    auto *dst = reinterpret_cast<uint8_t *>(&result);

    for (std::size_t i = 0; i < sizeof(T); i++)
        dst[sizeof(T) - 1 - i] = src[i];

    return result;
}

} // namespace cubic::nbt::details

#define NBT_TO_HOST(value) cubic::nbt::details::byte_swap(value)
#define HOST_TO_NBT(value) cubic::nbt::details::byte_swap(value)

#elif __BYTE_ORDER == __BIG_ENDIAN

#define NBT_TO_HOST(value) (value)
#define HOST_TO_NBT(value) (value)

#else

#error "Unsupported byte order"

#endif
