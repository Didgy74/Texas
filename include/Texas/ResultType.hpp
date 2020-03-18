#pragma once

namespace Texas
{
    /*
        Enum for categorizing errors.
    */
    enum class ResultType : char
    {
        Success,
        UnknownError,

        CouldNotOpenFile,
        CorruptFileData,
        FileNotSupported,
        InvalidLibraryUsage,
        PrematureEndOfFile,
    };
}