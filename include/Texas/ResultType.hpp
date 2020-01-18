#pragma once

#include <cstdint>

namespace Texas
{
    /*
        An enum for categorizing errors.
    */
    enum class ResultType : std::uint8_t
    {
        Success,

        CouldNotOpenFile,
        CorruptFileData,
        FileNotSupported,
        InvalidLibraryUsage,
        PixelFormatNotSupported,
        PrematureEndOfFile,
    };
}

