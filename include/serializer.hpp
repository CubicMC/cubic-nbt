#pragma once

#include "NBT/options.h"
#include <cassert>
#include <cstdint>
#include <map>
#include <string_view>
#include <unordered_map>
#include <vector>

CUBIC_WRAP_BEGIN

// TODO: Move this to either the main library or a separate one
class Serializer {
public:
    virtual ~Serializer() = default;

    virtual auto write_data(const uint8_t *data, size_t size) -> bool = 0;

    template<typename T>
    void write_raw(const T &value);

    template<typename T>
    void write(const T &value);

    template<typename T>
    void write_object(const T &object);

    void write_string(const std::string_view &string);

    template<typename Key, typename Value>
    void write_map(const std::map<Key, Value> &map, bool write_size = true);

    template<typename Key, typename Value>
    void write_map(const std::unordered_map<Key, Value> &map, bool write_size = true);

    template<typename T>
    void write_array(const std::vector<T> &array, bool write_size = true);

    template<typename T>
    void write_array(const T *array, uint32_t size);
};

template<typename T>
void Serializer::write_raw(const T &value)
{
    UNUSED bool success = this->write_data(reinterpret_cast<const uint8_t *>(&value), sizeof(T));
    assert(success);
}

template<typename T>
void Serializer::write_object(const T &object)
{
    T::serialize(this, object);
}

template<typename T>
void Serializer::write(const T &value)
{
    if constexpr (std::is_trivial<T>::value)
        write_raw(value);
    else
        write_object(value);
}

template<typename Key, typename Value>
void Serializer::write_map(const std::map<Key, Value> &map, bool write_size)
{
    if (write_size)
        write_raw(static_cast<int32_t>(map.size()));

    for (const auto &[key, value] : map) {
        if constexpr (std::is_trivial<Key>::value)
            write_raw<Key>(key);
        else
            write_object<Key>(key);

        if constexpr (std::is_trivial<Value>::value)
            write_raw<Value>(value);
        else
            write_object<Value>(value);
    }
}

template<typename Key, typename Value>
void Serializer::write_map(const std::unordered_map<Key, Value> &map, bool write_size)
{
    if (write_size)
        write_raw(static_cast<int32_t>(map.size()));

    for (const auto &[key, value] : map) {
        if constexpr (std::is_trivial<Key>::value)
            write_raw<Key>(key);
        else
            write_object<Key>(key);

        if constexpr (std::is_trivial<Value>::value)
            write_raw<Value>(value);
        else
            write_object<Value>(value);
    }
}

template<typename T>
void Serializer::write_array(const std::vector<T> &array, bool write_size)
{
    if (write_size)
        write_raw<uint32_t>(static_cast<uint32_t>(array.size()));

    for (const auto &value : array) {
        if constexpr (std::is_trivial<T>::value)
            write_raw<T>(value);
        else
            write_object<T>(value);
    }
}

template<typename T>
void Serializer::write_array(const T *array, uint32_t size)
{
    UNUSED bool success = write_data(reinterpret_cast<const uint8_t *>(array), size * sizeof(T));
    assert(success);
}

CUBIC_WRAP_END
