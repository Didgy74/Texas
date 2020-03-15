#pragma once

namespace Texas
{
    /*
        An enum for categorizing errors.
    */
    enum class ResultType : unsigned char
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