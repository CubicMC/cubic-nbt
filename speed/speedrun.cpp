#include <cubic-nbt/options.h>
#include <cubic-nbt/tag.hpp>
#include <cubic-parsing/parser.hpp>
#include <cubic-parsing/serializer.hpp>
#include <nbt.hpp>
#include <numeric>

#include <array>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <fstream>
#include <iostream>

class FileParser : public cubic::parsing::Parser {
public:
    FileParser(std::istream &input):
        stream(input)
    {
        this->buffer = std::vector<uint8_t>(
            std::istreambuf_iterator<char>(this->stream), std::istreambuf_iterator<char>()
        );
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

class MemorySerializer : public cubic::parsing::Serializer {
public:
    auto write_data(const uint8_t *data, size_t size) -> bool final
    {
        this->buffer.insert(this->buffer.end(), data, data + size);
        return true;
    }

    std::vector<uint8_t> buffer;
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
    FileParser parser(file);

    std::signal(SIGINT, signal_handler);

    auto size = static_cast<double>(parser.buffer.size()) / 1024.F / 1024.F;
    std::cout << "My nbt lib Shiny :)" << std::endl;
    std::cout << "(ctrl+c to stop or wait until this parse the file "
              << TO_STRING(NBT_SPEEDRUN_CYCLE) << " times)" << std::endl;
    std::cout << "Serializing a " << size << " mo file" << std::endl;

    {
        std::cout << "- Reading from memory" << std::endl;

        std::array<long, NBT_SPEEDRUN_CYCLE> loop_time_recording;
        size_t i = 0;

        auto loop_start = high_resolution_clock::now();
        for (i = 0; i < NBT_SPEEDRUN_CYCLE && RUNNING; i++) {
            cubic::nbt::Nbt tag;
            auto start = high_resolution_clock::now();
            parser.read(tag);
            auto end = high_resolution_clock::now();

            auto &diff = loop_time_recording.at(i);
            diff = duration_cast<milliseconds>(end - start).count();
            auto sec = static_cast<double>(diff) / 1000.F;

            parser.offset = 0;
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
                  << static_cast<double>(accumulate(
                         loop_time_recording.begin(), loop_time_recording.begin() + i, 0L
                     ))
                / 1000.F
                  << "s (with time calc "
                  << static_cast<double>(duration_cast<milliseconds>(loop_end - loop_start).count())
                / 1000.F
                  << "s)" << std::endl;
    }
    // Same but for writting to memory
    cubic::nbt::Nbt tag;
    parser.read(tag);

    {
        std::cout << "- Writing to memory" << std::endl;
        std::array<long, NBT_SPEEDRUN_CYCLE> loop_time_recording;
        size_t i = 0;

        auto loop_start = high_resolution_clock::now();
        for (i = 0; i < NBT_SPEEDRUN_CYCLE && RUNNING; i++) {
            MemorySerializer serializer;
            auto start = high_resolution_clock::now();
            serializer.write(tag);
            auto end = high_resolution_clock::now();

            auto &diff = loop_time_recording.at(i);
            diff = duration_cast<milliseconds>(end - start).count();
            auto sec = static_cast<double>(diff) / 1000.F;

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
                  << static_cast<double>(accumulate(
                         loop_time_recording.begin(), loop_time_recording.begin() + i, 0L
                     ))
                / 1000.F
                  << "s (with time calc "
                  << static_cast<double>(duration_cast<milliseconds>(loop_end - loop_start).count())
                / 1000.F
                  << "s)" << std::endl;
    }

    return 0;
}
