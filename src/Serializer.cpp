#include <NBT/options.h>
#include <parser.hpp>

CUBIC_WRAP_BEGIN

void Parser::read_string(std::string &string)
{
    uint32_t size = 0;
    this->read_raw(size);
    string.resize(size);
    this->read_data(reinterpret_cast<uint8_t *>(string.data()), size);
}

CUBIC_WRAP_END
