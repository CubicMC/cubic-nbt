#include <cubic-nbt/endian.hpp>
#include <cubic-nbt/options.h>
#include <cubic-nbt/tag.hpp>
#include <cubic-nbt/type.hpp>

#include <cubic-parsing/parser.hpp>
#include <cubic-parsing/serializer.hpp>

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <variant>

namespace cubic::nbt {
namespace details {

static auto make_tag(tag_type type) -> Tag
{
    switch (type) {
    case tag_type::Byte:
        return TagByte{};
    case tag_type::Short:
        return TagShort{};
    case tag_type::Int:
        return TagInt{};
    case tag_type::Long:
        return TagLong{};
    case tag_type::Float:
        return TagFloat{};
    case tag_type::Double:
        return TagDouble{};
    case tag_type::ByteArray:
        return TagByteArray{};
    case tag_type::String:
        return TagString{};
    case tag_type::List:
        return TagList{};
    case tag_type::Compound:
        return TagCompound{};
    case tag_type::IntArray:
        return TagIntArray{};
    case tag_type::LongArray:
        return TagLongArray{};
    default:
        throw std::runtime_error("Invalid tag type.");
    }
}

#define MAKE_TAG_LIST_CASE(type)                \
    case tag_type::type: {                      \
        auto result = std::vector<Tag##type>(); \
        result.reserve(size);                   \
        return result;                          \
    }

static auto make_tag_list(tag_type type, size_t size) -> TagList
{
    switch (type) {
        MAKE_TAG_LIST_CASE(Byte)
        MAKE_TAG_LIST_CASE(Short)
        MAKE_TAG_LIST_CASE(Int)
        MAKE_TAG_LIST_CASE(Long)
        MAKE_TAG_LIST_CASE(Float)
        MAKE_TAG_LIST_CASE(Double)
        MAKE_TAG_LIST_CASE(ByteArray)
        MAKE_TAG_LIST_CASE(String)
        MAKE_TAG_LIST_CASE(List)
        MAKE_TAG_LIST_CASE(Compound)
        MAKE_TAG_LIST_CASE(IntArray)
        MAKE_TAG_LIST_CASE(LongArray)
    default:
        if (size > 0)
            throw std::runtime_error("Invalid tag type in list.");
        return TagList{};
    }
}

#undef MAKE_TAG_LIST_CASE

} // namespace details

NetworkNbt::NetworkNbt(TagCompound &&compound):
    data(std::move(compound))
{
}

NetworkNbt::NetworkNbt(Nbt &nbt):
    data(std::move(nbt.data))
{
}

Nbt::Nbt(TagString label, TagCompound compound):
    name(std::move(label)),
    data(std::move(compound))
{
}

Nbt::Nbt(NetworkNbt &nbt):
    name(""),
    data(nbt.data)
{
}

void TagString::serialize(parsing::Serializer *serializer, const TagString &tag)
{
    serializer->write<uint16_t>(HOST_TO_NBT(static_cast<uint16_t>(tag.size())));
    serializer->write_array<char>(tag.data(), static_cast<uint32_t>(tag.size()));
}

void TagString::parse(parsing::Parser *parser, TagString &tag)
{
    uint16_t size = 0;
    parser->read<uint16_t>(size);
    size = NBT_TO_HOST(size);
    tag.resize(size);
    parser->read_array<char>(tag.data(), size);
}

void TagList::serialize(parsing::Serializer *serializer, const TagList &tag)
{
    if (tag.size() <= 0) {
        serializer->write_raw<tag_type>(tag_type::End);
        serializer->write_raw<int32_t>(0);
        return;
    }
    assert(tag.size() > 0 && tag.type != tag_type::End);
    assert(tag.type > tag_type::End && tag.type <= tag_type::LongArray);
    serializer->write_raw<tag_type>(tag.type);
    std::visit(
        [&serializer](auto &inner_tag) {
            using vector_type = typename std::remove_reference<decltype(inner_tag)>::type;
            using inner_type = typename vector_type::value_type;

            serializer->write_raw<int32_t>(HOST_TO_NBT(static_cast<int32_t>(inner_tag.size())));
            if constexpr (std::is_arithmetic<inner_type>::value
                          && sizeof(inner_type) > sizeof(uint8_t)) {
                for (auto &value : inner_tag)
                    serializer->write_raw<inner_type>(HOST_TO_NBT(value));
            } else
                serializer->write_array<inner_type>(inner_tag, false);
        },
        tag
    );
}

void TagList::parse(parsing::Parser *parser, TagList &tag)
{
    int32_t size = 0;
    tag_type type;
    parser->read<tag_type>(type);
    parser->read<int32_t>(size);
    size = NBT_TO_HOST(size);
    if (size > 0 && type == tag_type::End)
        throw std::runtime_error("Invalid tag type in list.");
    if (size <= 0)
        return;

    assert(type > tag_type::End && type <= tag_type::LongArray);
    tag = details::make_tag_list(type, static_cast<size_t>(size));
    tag.type = type;

    switch (tag.type) {
    case tag_type::Byte:
        parser->read_array(std::get<std::vector<TagByte>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Short:
        parser->read_array(std::get<std::vector<TagShort>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Int:
        parser->read_array(std::get<std::vector<TagInt>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Long:
        parser->read_array(std::get<std::vector<TagLong>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Float:
        parser->read_array(std::get<std::vector<TagFloat>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Double:
        parser->read_array(std::get<std::vector<TagDouble>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::ByteArray:
        parser->read_array(std::get<std::vector<TagByteArray>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::String:
        parser->read_array(std::get<std::vector<TagString>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::List:
        parser->read_array(std::get<std::vector<TagList>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::Compound:
        parser->read_array(std::get<std::vector<TagCompound>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::IntArray:
        parser->read_array(std::get<std::vector<TagIntArray>>(tag), static_cast<uint32_t>(size));
        break;
    case tag_type::LongArray:
        parser->read_array(std::get<std::vector<TagLongArray>>(tag), static_cast<uint32_t>(size));
        break;
    default:
        throw std::runtime_error("Invalid tag type in list.");
    }

    std::visit(
        [](auto &inner_tag) {
            using vector_type = std::remove_reference_t<decltype(inner_tag)>;
            using inner_type = typename vector_type::value_type;

            if constexpr (std::is_arithmetic<inner_type>::value
                          && sizeof(inner_type) > sizeof(uint8_t)) {
                for (auto &value : inner_tag)
                    value = NBT_TO_HOST(value);
            }
        },
        tag
    );
}

void TagCompound::serialize(parsing::Serializer *serializer, const TagCompound &tag)
{
    for (const auto &[key, value] : tag) {
        serializer->write<tag_type>(value.type());
        serializer->write(key);
        serializer->write(value);
    }
    serializer->write(tag_type::End);
}

void TagCompound::parse(parsing::Parser *parser, TagCompound &tag)
{
    tag_type type;
    parser->read(type);
    for (; type != tag_type::End; parser->read(type)) {
        TagString key;
        Tag value = details::make_tag(type);
        parser->read(key);
        parser->read(value);
        tag.insert(std::make_pair(std::move(key), std::move(value)));
    }
}

void Tag::serialize(parsing::Serializer *serializer, const Tag &tag)
{
    std::visit(
        [&serializer](auto &inner_tag) {
            if constexpr (std::is_trivial<
                              typename std::remove_reference<decltype(inner_tag)>::type>::value)
                serializer->write_raw(HOST_TO_NBT(inner_tag));
            else
                serializer->write_object(inner_tag);
        },
        tag
    );
}

void Tag::parse(parsing::Parser *parser, Tag &tag)
{
    std::visit(
        [&parser](auto &inner_tag) {
            using inner_type = typename std::remove_reference<decltype(inner_tag)>::type;

            parser->read(inner_tag);

            if constexpr (std::is_arithmetic<inner_type>::value
                          && sizeof(inner_type) > sizeof(uint8_t))
                inner_tag = NBT_TO_HOST(inner_tag);
        },
        tag
    );
}

void NetworkNbt::serialize(parsing::Serializer *serializer, const NetworkNbt &nbt)
{
    serializer->write(tag_type::Compound);
    serializer->write(nbt.data);
}

void NetworkNbt::parse(parsing::Parser *parser, NetworkNbt &nbt)
{
    tag_type type;
    parser->read(type);

    if (type != tag_type::Compound)
        throw std::runtime_error("Invalid tag type.");

    parser->read(nbt.data);
}

void Nbt::serialize(parsing::Serializer *serializer, const Nbt &nbt)
{
    serializer->write(tag_type::Compound);
    serializer->write(nbt.name);
    serializer->write(nbt.data);
}

void Nbt::parse(parsing::Parser *parser, Nbt &nbt)
{
    tag_type type;
    parser->read(type);

    if (type != tag_type::Compound)
        throw std::runtime_error("Invalid tag type.");

    parser->read(nbt.name);
    parser->read(nbt.data);
}

auto Tag::type() const -> tag_type
{
    auto result = static_cast<tag_type>(this->index() + 1);
    assert(result > tag_type::End && result <= tag_type::LongArray);
    return result;
}

auto TagList::size() const -> size_t
{
    return std::visit([](const auto &tag) { return tag.size(); }, *this);
}

} // namespace cubic::nbt
