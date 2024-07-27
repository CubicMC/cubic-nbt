#pragma once

#include "NBT/options.h"
#include <cassert>
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

CUBIC_WRAP_BEGIN

// TODO: Move this to either the main library or a separate one
class Parser {
public:
    virtual ~Parser() = default;

    virtual auto read_data(uint8_t *data, size_t size) -> bool = 0;

    template<typename T>
    void read_raw(T &value);

    template<typename T>
    void read(T &value);

    template<typename T>
    void read_object(T &object);

    void read_string(std::string &string);

    template<typename Key, typename Value>
    void read_map(std::map<Key, Value> &map, uint32_t size = 0);

    template<typename Key, typename Value>
    void read_map(std::unordered_map<Key, Value> &map, uint32_t size = 0);

    template<typename T>
    void read_array(std::vector<T> &array, uint32_t size = 0);

    template<typename T>
    void read_array(T *array, uint32_t size);
};

template<typename T>
void Parser::read_raw(T &value)
{
    UNUSED bool success = this->read_data(reinterpret_cast<uint8_t *>(&value), sizeof(T));
    assert(success);
}

template<typename T>
void Parser::read_object(T &object)
{
    T::parse(this, object);
}

template<typename T>
void Parser::read(T &value)
{
    if constexpr (std::is_trivial<T>::value)
        read_raw(value);
    else
        read_object(value);
}

template<typename Key, typename Value>
void Parser::read_map(std::map<Key, Value> &map, uint32_t size)
{
    if (size == 0)
        read_raw<uint32_t>(size);

    for (uint32_t i = 0; i < size; i++) {
        Key key;
        if constexpr (std::is_trivial<Key>::value)
            read_raw<Key>(key);
        else
            read_object<Key>(key);

        if constexpr (std::is_trivial<Value>::value)
            read_raw<Value>(map[key]);
        else
            read_object<Value>(map[key]);
    }
}

template<typename Key, typename Value>
void Parser::read_map(std::unordered_map<Key, Value> &map, uint32_t size)
{
    if (size == 0)
        read_raw<uint32_t>(size);

    for (uint32_t i = 0; i < size; i++) {
        Key key;
        if constexpr (std::is_trivial<Key>::value)
            read_raw<Key>(key);
        else
            read_object<Key>(key);

        if constexpr (std::is_trivial<Value>::value)
            read_raw<Value>(map[key]);
        else
            read_object<Value>(map[key]);
    }
}

template<typename T>
void Parser::read_array(std::vector<T> &array, uint32_t size)
{
    if (size == 0)
        read_raw<uint32_t>(size);

    for (uint32_t i = 0; i < size; i++) {
        T value;
        if constexpr (std::is_trivial<T>::value)
            read_raw<T>(value);
        else
            read_object<T>(value);

        array.emplace_back(std::move(value));
    }
}

template<typename T>
void Parser::read_array(T *array, uint32_t size)
{
    if (size != 0)
        assert(array != nullptr);

    for (uint32_t i = 0; i < size; i++) {
        if constexpr (std::is_trivial<T>::value)
            read_raw<T>(array[i]);
        else
            read_object<T>(array[i]);
    }
}

CUBIC_WRAP_END
