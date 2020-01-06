#pragma once

#include <cstdint>

namespace Texas
{
    enum class ResultType : std::uint8_t;
}

enum class Texas::ResultType : std::uint8_t
{
    Success,

    CouldNotOpenFile,
    CorruptFileData,
    FileNotSupported,
    InvalidInputParameter,
    PixelFormatNotSupported,
    PrematureEndOfFile,
    NoIdea,

    COUNT
};