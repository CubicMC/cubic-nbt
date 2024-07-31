#pragma once

#include <cubic-nbt/endian.hpp>
#include <cubic-nbt/options.h>
#include <cubic-nbt/type.hpp>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <string>
#include <variant>
#include <vector>

// namespace cubic::parsing {
// class Parser;
// class Serializer;
// } // namespace cubic::parsing
#include <cubic-parsing/parser.hpp>
#include <cubic-parsing/serializer.hpp>

namespace cubic::nbt {

struct Tag;

template<typename T>
struct TagArray;

using TagByte = int8_t;
using TagShort = int16_t;
using TagInt = int32_t;
using TagLong = int64_t;
using TagFloat = float;
using TagDouble = double;
using TagByteArray = TagArray<TagByte>;
struct TagString;
struct TagList;
using TagIntArray = TagArray<TagInt>;
using TagLongArray = TagArray<TagLong>;
struct TagCompound;

template<typename T>
struct CUBIC_NBT_API TagArray : public std::vector<T> {
    using std::vector<T>::vector;

    TagArray(std::vector<T> &&original) noexcept:
        std::vector<T>(std::move(original))
    {
    }
    TagArray(const std::vector<T> &original):
        std::vector<T>(original)
    {
        std::cout << "TagArray: Copy constructor" << std::endl;
    }
    auto operator=(const std::vector<T> &original) -> TagArray<T> &
    {
        std::vector<T>::operator=(original);
        std::cout << "TagArray: Copy assignment" << std::endl;
        return *this;
    }
    auto operator=(std::vector<T> &&original) noexcept -> TagArray<T> &
    {
        std::vector<T>::operator=(std::move(original));
        return *this;
    }

    static void parse(parsing::Parser *parser, TagArray<T> &tag);
    static void serialize(parsing::Serializer *serializer, const TagArray<T> &tag);
};

struct CUBIC_NBT_API TagString : public std::string {
    using std::string::string;

    TagString() = default;
    ~TagString() = default;

    TagString(TagString &&original) noexcept:
        std::string(std::move(original))
    {
    }
    TagString(const TagString &original):
        std::string(original)
    {
        std::cout << "TagString: Copy constructor" << std::endl;
    }
    auto operator=(const TagString &original) -> TagString &
    {
        std::string::operator=(original);
        std::cout << "TagString: Copy assignment" << std::endl;
        return *this;
    }
    auto operator=(TagString &&original) noexcept -> TagString &
    {
        std::string::operator=(std::move(original));
        return *this;
    }

    static void parse(parsing::Parser *parser, TagString &tag);
    static void serialize(parsing::Serializer *serializer, const TagString &tag);
};

struct CUBIC_NBT_API TagList
    : public std::variant<
          std::vector<TagByte>, std::vector<TagShort>, std::vector<TagInt>, std::vector<TagLong>,
          std::vector<TagFloat>, std::vector<TagDouble>, std::vector<TagByteArray>,
          std::vector<TagString>, std::vector<TagList>, std::vector<TagCompound>,
          std::vector<TagIntArray>, std::vector<TagLongArray>> {
    using variant::variant;

    TagList(TagList &&original) noexcept:
        variant(std::move(original)),
        type(original.type)
    {
    }
    TagList(const TagList &original):
        variant(original),
        type(original.type)
    {
        std::cout << "TagList: Copy constructor" << std::endl;
    }
    auto operator=(const TagList &original) -> TagList &
    {
        variant::operator=(original);
        this->type = original.type;
        std::cout << "TagList: Copy assignment" << std::endl;
        return *this;
    }
    auto operator=(TagList &&original) noexcept -> TagList &
    {
        variant::operator=(std::move(original));
        this->type = original.type;
        return *this;
    }

    static void parse(parsing::Parser *parser, TagList &tag);
    static void serialize(parsing::Serializer *serializer, const TagList &tag);

    [[nodiscard]]
    auto size() const -> size_t;

    tag_type type;
};

struct CUBIC_NBT_API TagCompound : public std::map<TagString, Tag> {
    using std::map<TagString, Tag>::map;

    TagCompound() = default;

    TagCompound(TagCompound &&original) noexcept:
        std::map<TagString, Tag>(std::move(original))
    {
    }
    TagCompound(const TagCompound &original):
        std::map<TagString, Tag>(original)
    {
        std::cout << "TagCompound: Copy constructor" << std::endl;
    }
    auto operator=(const TagCompound &original) -> TagCompound &
    {
        std::map<TagString, Tag>::operator=(original);
        std::cout << "TagCompound: Copy assignment" << std::endl;
        return *this;
    }
    auto operator=(TagCompound &&original) noexcept -> TagCompound &
    {
        std::map<TagString, Tag>::operator=(std::move(original));
        return *this;
    }

    static void parse(parsing::Parser *parser, TagCompound &tag);
    static void serialize(parsing::Serializer *serializer, const TagCompound &tag);
};

struct CUBIC_NBT_API Tag
    : public std::variant<
          TagByte, TagShort, TagInt, TagLong, TagFloat, TagDouble, TagByteArray, TagString, TagList,
          TagCompound, TagIntArray, TagLongArray> {
    using variant::variant;

    Tag() = default;

    Tag(Tag &&original) noexcept:
        variant(std::move(original))
    {
    }
    Tag(const Tag &original):
        variant(original)
    {
        std::cout << "Tag: Copy constructor" << std::endl;
    }
    auto operator=(const Tag &original) -> Tag &
    {
        variant::operator=(original);
        std::cout << "Tag: Copy assignment" << std::endl;
        return *this;
    }
    auto operator=(Tag &&original) noexcept -> Tag &
    {
        variant::operator=(std::move(original));
        return *this;
    }

    [[nodiscard]]
    auto type() const -> tag_type;

    static void parse(parsing::Parser *parser, Tag &tag);
    static void serialize(parsing::Serializer *serializer, const Tag &tag);
};

struct Nbt;

struct CUBIC_NBT_API NetworkNbt {
    NetworkNbt() = default;
    NetworkNbt(TagCompound &&compound);
    // This may be a really bad idea performance wise
    // Maybe add a specific container for it
    explicit NetworkNbt(Nbt &nbt);
    ~NetworkNbt() = default;

    TagCompound data;

    static void parse(parsing::Parser *parser, NetworkNbt &nbt);
    static void serialize(parsing::Serializer *serializer, const NetworkNbt &nbt);
};

struct CUBIC_NBT_API Nbt {
    Nbt() = default;
    Nbt(TagString label, TagCompound compound);
    // This may be a really bad idea performance wise
    // Maybe add a specific container for it
    explicit Nbt(NetworkNbt &nbt);
    ~Nbt() = default;

    Nbt(const Nbt &original):
        name(original.name),
        data(original.data)
    {
        std::cout << "Nbt: Copy constructor" << std::endl;
    }
    auto operator=(const Nbt &nbt) -> Nbt &
    {
        name = nbt.name;
        data = nbt.data;
        std::cout << "Nbt: Copy assignment" << std::endl;
        return *this;
    }

    TagString name;
    TagCompound data;

    static void parse(parsing::Parser *parser, Nbt &nbt);
    static void serialize(parsing::Serializer *serializer, const Nbt &nbt);
};

template<typename T>
void TagArray<T>::serialize(parsing::Serializer *serializer, const TagArray<T> &tag)
{
    serializer->write_raw<int32_t>(HOST_TO_NBT(static_cast<int32_t>(tag.size())));
    for (const auto &value : tag) {
        if constexpr (std::is_trivial<T>::value)
            serializer->write_raw<T>(HOST_TO_NBT(value));
        else
            serializer->write_object(value);
    }
}

template<typename T>
void TagArray<T>::parse(parsing::Parser *parser, TagArray<T> &tag)
{
    int32_t size;
    parser->read_raw<int32_t>(size);
    size = NBT_TO_HOST(size);
    tag.resize(size);
    for (auto &value : tag) {
        if constexpr (std::is_trivial<T>::value) {
            parser->read_raw(value);
            value = NBT_TO_HOST(value);
        } else
            parser->read_object(value);
    }
}
} // namespace cubic::nbt
