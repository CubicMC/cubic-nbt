#pragma once

#include <cubic-nbt/options.h>

#include <cstdint>
#include <ostream>
#include <string_view>

namespace cubic::nbt {

class CUBIC_NBT_API tag_type final {
public:
    enum value : uint8_t {
        End = 0,
        Byte = 1,
        Short = 2,
        Int = 3,
        Long = 4,
        Float = 5,
        Double = 6,
        ByteArray = 7,
        String = 8,
        List = 9,
        Compound = 10,
        IntArray = 11,
        LongArray = 12
    };

    tag_type() = default;
    constexpr tag_type(value val) noexcept:
        _value(val)
    {
    }
    constexpr tag_type(uint64_t val) noexcept:
        _value(static_cast<value>(val))
    {
    }

    constexpr operator value() const
    {
        return _value;
    }

    explicit operator int() const
    {
        return static_cast<int>(_value);
    }
    explicit operator bool() const = delete;

    [[nodiscard]]
    auto to_string() const -> std::string_view
    {
        switch (this->_value) {
        case value::End:
            return "End";
        case value::Byte:
            return "Byte";
        case value::Short:
            return "Short";
        case value::Int:
            return "Int";
        case value::Long:
            return "Long";
        case value::Float:
            return "Float";
        case value::Double:
            return "Double";
        case value::ByteArray:
            return "ByteArray";
        case value::String:
            return "String";
        case value::List:
            return "List";
        case value::Compound:
            return "Compound";
        case value::IntArray:
            return "IntArray";
        case value::LongArray:
            return "LongArray";
        default:
            return "Unknown";
        }
    }

private:
    value _value;
};
static_assert(std::is_trivial<tag_type>::value, "tag_type must be trivial");

} // namespace cubic::nbt

inline auto operator<<(std::ostream &os, const cubic::nbt::tag_type &type) -> std::ostream &
{
    return os << type.to_string();
}
