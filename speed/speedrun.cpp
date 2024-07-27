#include "NBT/options.h"
#include <nbt.hpp>
#include <NBT/Tag.hpp>
#include <numeric>
#include <parser.hpp>
#include <serializer.hpp>

#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <iostream>

#include <gzip/decompress.hpp>
#include <gzip/utils.hpp>

CUBIC_WRAP_USING;

class FileParser : public Parser {
public:
    FileParser(std::istream &input):
        stream(input)
    {
        auto start = this->stream.tellg();
        std::array<char, 10> header;
        this->stream.read(header.data(), 10);
        this->stream.seekg(start);

        if (gzip::is_compressed(header.data(), 10)) {
            std::vector<char> compressed = std::vector<char>(
                std::istreambuf_iterator<char>(this->stream), std::istreambuf_iterator<char>()
            );
            gzip::Decompressor decompressor;
            auto end = decompressor.decompress(this->buffer, compressed.data(), compressed.size());
            this->stream.seekg(static_cast<int64_t>(end));
        }
    }

    auto read_data(uint8_t *data, size_t size) -> bool final
    {
        if (this->buffer.size() <= 0) {
            this->stream.read(reinterpret_cast<char *>(data), static_cast<int64_t>(size));
            return true;
        }
        if (this->buffer.size() - this->offset < size)
            return false;

        std::copy(
            this->buffer.begin() + static_cast<int64_t>(this->offset),
            this->buffer.begin() + static_cast<int64_t>(this->offset + size), data
        );
        this->offset += size;

        return true;
    }

    std::vector<uint8_t> buffer;
    uint64_t offset = 0;
    std::istream &stream;
};

bool RUNNING = true;

void signal_handler(int /*unused*/)
{
    RUNNING = false;
}

#ifndef NBT_SPEEDRUN_CYCLE
#define NBT_SPEEDRUN_CYCLE 1000
#endif

#define TO_STRING_IMPL(x) #x
#define TO_STRING(x) TO_STRING_IMPL(x)

auto main() -> int
{
    using namespace std::chrono;
    using std::accumulate;

    auto file = std::ifstream("./speed/house.nbt", std::fstream::in | std::fstream::binary);
    FileParser serializer(file);

    std::signal(SIGINT, signal_handler);

    auto size = static_cast<double>(serializer.buffer.size()) / 1024.F / 1024.F;
    std::cout << "My nbt lib Shiny :)" << std::endl;
    std::cout << "(ctrl+c to stop or wait until this parse the file "
              << TO_STRING(NBT_SPEEDRUN_CYCLE) << " times)" << std::endl;
    std::cout << "Serializing a " << size << " mo file" << std::endl;

    std::array<long, NBT_SPEEDRUN_CYCLE> loop_time_recording;
    size_t i = 0;

    auto loop_start = high_resolution_clock::now();
    for (i = 0; i < NBT_SPEEDRUN_CYCLE && RUNNING; i++) {
        nbt::Nbt tag2;
        auto start = high_resolution_clock::now();
        serializer.read(tag2);
        auto end = high_resolution_clock::now();

        auto &diff = loop_time_recording.at(i);
        diff = duration_cast<milliseconds>(end - start).count();
        auto sec = static_cast<double>(diff) / 1000.F;

        serializer.offset = 0;
        if (diff > 0) {
            std::cout << "\r                                            \r"
                      << size * static_cast<double>(i)
                    / (static_cast<double>(accumulate(
                           loop_time_recording.begin(), loop_time_recording.begin() + i, 0L
                       ))
                       / 1000.F)
                      << " mo/s - " << sec << "s";
            std::cout.flush();
        }
    }
    auto loop_end = high_resolution_clock::now();
    std::cout << "\r                                            \r"
              << size * static_cast<double>(i)
            / (static_cast<double>(
                   accumulate(loop_time_recording.begin(), loop_time_recording.begin() + i, 0L)
               )
               / 1000.F)
              << " mo/s - total time "
              << static_cast<double>(
                     accumulate(loop_time_recording.begin(), loop_time_recording.begin() + i, 0L)
                 )
            / 1000.F
              << "s (with time calc "
              << static_cast<double>(duration_cast<milliseconds>(loop_end - loop_start).count())
            / 1000.F
              << "s)" << std::endl;

    return 0;
}
