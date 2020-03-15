#pragma once

namespace Texas
{
    /*
        An enum for categorizing errors.
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