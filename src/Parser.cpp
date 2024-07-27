#include <NBT/options.h>
#include <serializer.hpp>
#include <string_view>

CUBIC_WRAP_BEGIN

void Serializer::write_string(const std::string_view &string)
{
    write_raw(static_cast<uint32_t>(string.size()));
    write_data(reinterpret_cast<const uint8_t *>(string.data()), string.size());
}

CUBIC_WRAP_END
