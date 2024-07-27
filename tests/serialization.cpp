#include <NBT/Tag.hpp>

#include <criterion/criterion.h>

TestSuite(tag_serialization, .timeout = 1);

CUBIC_WRAP_USING;

class MemoryBuffer : public Parser, public Serializer {
public:
    auto write_data(const uint8_t *raw, size_t size) -> bool override;
    auto read_data(uint8_t *raw, size_t size) -> bool override;

    std::vector<uint8_t> vector;
    size_t index{};
};

auto MemoryBuffer::write_data(const uint8_t *raw, size_t size) -> bool
{
    this->vector.insert(this->vector.end(), raw, raw + size);
    return true;
}

auto MemoryBuffer::read_data(uint8_t *raw, size_t size) -> bool
{
    if (this->index + size > this->vector.size())
        return false;

    std::memcpy(raw, this->vector.data() + this->index, size);
    this->index += size;
    return true;
}

Test(tag_serialization, serialize_hello_world)
{
    uint8_t hello_world[] = { 0x0a, 0x00, 0x0b, 0x68, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                              0x77, 0x6f, 0x72, 0x6c, 0x64, 0x03, 0x00, 0x04, 0x6e,
                              0x61, 0x6d, 0x65, 0x00, 0x00, 0x00, 0x2a, 0x00 };

    nbt::Nbt tag = { "hello world", { { nbt::TagString("name"), nbt::TagInt(42) } } };
    nbt::TagString name("hello");

    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(hello_world, buffer.vector.data(), sizeof(hello_world));
}

Test(tag_serialization, serialize_byte)
{
    uint8_t byte_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x01, 0x00, 0x09, 0x62,
                            0x79, 0x74, 0x65, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x2a, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("byte_test"), nbt::TagByte(42) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(byte_data, buffer.vector.data(), sizeof(byte_data));
}

Test(tag_serialization, serialize_short)
{
    uint8_t short_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x02, 0x00, 0x0a, 0x73, 0x68,
                             0x6f, 0x72, 0x74, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x00, 0x2a, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("short_test"), nbt::TagShort(42) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(short_data, buffer.vector.data(), sizeof(short_data));
}

Test(tag_serialization, serialize_int)
{
    uint8_t int_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x03, 0x00, 0x08, 0x69, 0x6e,
                           0x74, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x00, 0x00, 0x00, 0x2a, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("int_test"), nbt::TagInt(42) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(int_data, buffer.vector.data(), sizeof(int_data));
}

Test(tag_serialization, serialize_long)
{
    uint8_t long_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x04, 0x00, 0x09,
                            0x6c, 0x6f, 0x6e, 0x67, 0x5f, 0x74, 0x65, 0x73, 0x74, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2a, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("long_test"), nbt::TagLong(42) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(long_data, buffer.vector.data(), sizeof(long_data));
}

Test(tag_serialization, serialize_float)
{
    uint8_t float_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x05, 0x00,
                             0x0a, 0x66, 0x6c, 0x6f, 0x61, 0x74, 0x5f, 0x74, 0x65,
                             0x73, 0x74, 0x42, 0x2a, 0xc2, 0x8f, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("float_test"), nbt::TagFloat(42.69) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(float_data, buffer.vector.data(), sizeof(float_data));
}

Test(tag_serialization, serialize_double)
{
    uint8_t double_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x06, 0x00, 0x0b,
                              0x64, 0x6f, 0x75, 0x62, 0x6c, 0x65, 0x5f, 0x74, 0x65, 0x73,
                              0x74, 0x40, 0x45, 0x58, 0x51, 0xeb, 0x85, 0x1e, 0xb8, 0x00 };

    nbt::Nbt tag = { "root", { { nbt::TagString("double_test"), nbt::TagDouble(42.69) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(double_data, buffer.vector.data(), sizeof(double_data));
}

Test(tag_serialization, serialize_byte_array)
{
    uint8_t byte_array_data[] = { 0x0a, 0x00, 0x04, 0x72, 0x6f, 0x6f, 0x74, 0x07, 0x00, 0x0f, 0x62,
                                  0x79, 0x74, 0x65, 0x5f, 0x61, 0x72, 0x72, 0x61, 0x79, 0x5f, 0x74,
                                  0x65, 0x73, 0x74, 0x00, 0x00, 0x00, 0x02, 0x2a, 0x2a, 0x00 };

    nbt::Nbt tag = { "root",
                     { { nbt::TagString("byte_array_test"), nbt::TagByteArray({ 42, 42 }) } } };
    MemoryBuffer buffer;

    buffer.write(tag);

    cr_assert_arr_eq(byte_array_data, buffer.vector.data(), sizeof(byte_array_data));
}
