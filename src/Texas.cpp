#include "PrivateAccessor.hpp"
#include "PrivateAccessor.hpp"
#include "Texas/Texas.hpp"
#include "Texas/detail/Exception.hpp"
#include "PrivateAccessor.hpp"
#include "Texas/Tools.hpp"
#include "NumericLimits.hpp"

#include <cstring>

#include "KTX.hpp"
#include "PNG.hpp"

#if !defined(TEXAS_ENABLE_KTX_READ) && !defined(TEXAS_ENABLE_PNG_READ)
#error Cannot compile Texas without enabling atleast one file-format.
#endif

Texas::ResultValue<Texas::FileInfo> Texas::parseBuffer(ConstByteSpan inputBuffer) noexcept
{
    return detail::PrivateAccessor::parseBuffer(inputBuffer);
}

Texas::Result Texas::loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept
{
    return detail::PrivateAccessor::loadImageData(file, dstBuffer, workingMemory);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(ConstByteSpan inputBuffer, Allocator& allocator) noexcept
{
    return detail::PrivateAccessor::loadFromBuffer(inputBuffer, &allocator);
}

Texas::ResultValue<Texas::FileInfo> Texas::detail::PrivateAccessor::parseBuffer(ConstByteSpan inputBuffer) noexcept
{
    // TODO: Check if input buffer is larger than 0.
    // 12 bytes is the largest file identifier we know of... So far.
    // TODO: Find out how small a file could possibly be and test if it's atleast that size.


    FileInfo memReqs{};

    // We test the file's identifier to see if it's KTX
    if (std::memcmp(inputBuffer.data(), KTX::identifier, sizeof(KTX::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_KTX_READ
        Result result = KTX::loadFromBuffer_Step1(inputBuffer, memReqs.m_textureInfo, memReqs.m_backendData.ktx);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSize(memReqs.textureInfo());
            return { static_cast<FileInfo&&>(memReqs) };
        }
        else
            return { result };
#else
        return { ResultType::FileNotSupported, 
                "Encountered a KTX-file. "
                "KTX support has not been enabled in this configuration." };
#endif
    }

    // Test if it's a PNG file
    if (std::memcmp(inputBuffer.data(), PNG::identifier, sizeof(PNG::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_PNG_READ
        Result result = PNG::loadFromBuffer_Step1(inputBuffer, memReqs.m_textureInfo, memReqs.m_workingMemoryRequired, memReqs.m_backendData.png);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSize(memReqs.textureInfo());
            return { static_cast<FileInfo&&>(memReqs) };
        }
        else
            return { result };
#else
        return { ResultType::FileNotSupported, "Encountered a PNG-file. PNG support has not been enabled in this configuration." };
#endif
    }

    return { ResultType::FileNotSupported, "Could not identify file-format of input, "
                                                               "or file-format is not supported." };
}

Texas::Result Texas::detail::PrivateAccessor::loadImageData(
    FileInfo const& file, 
    ByteSpan dstBuffer, 
    ByteSpan workingMem) noexcept
{
    if (dstBuffer.data() == nullptr)
        return { ResultType::InvalidLibraryUsage, "You need to send in a destination buffer." };
    if (dstBuffer.size() < file.memoryRequired())
        return { ResultType::InvalidLibraryUsage, 
                 "Destination buffer is not equal to or higher than Texas::MemReqs::memoryRequired(). "
                 "Cannot fit image data in this buffer." };
    if (file.workingMemoryRequired() > 0)
    {
        if (workingMem.data() == nullptr)
            return { ResultType::InvalidLibraryUsage, "Cannot pass nullptr for working-memory when loading image-data requires working-memory." };
        else if (workingMem.size() == 0)
            return { ResultType::InvalidLibraryUsage, "Cannot pass working-memory with size 0 when loading image-data requires working-memory." };
        else if (workingMem.size() < file.workingMemoryRequired())
            return { ResultType::InvalidLibraryUsage, "Working-memory passed in is not large enough to load the image-data." };
    }   

#ifdef TEXAS_ENABLE_KTX_READ
    if (file.textureInfo().fileFormat == FileFormat::KTX)
    {
        return detail::KTX::loadFromBuffer_Step2(file.textureInfo(), file.m_backendData.ktx, dstBuffer, workingMem);
    }
#endif

#ifdef TEXAS_ENABLE_PNG_READ
    if (file.textureInfo().fileFormat == FileFormat::PNG)
    {
        return detail::PNG::loadFromBuffer_Step2(file.textureInfo(), file.m_backendData.png, dstBuffer, workingMem);
    }
#endif

    return { ResultType::InvalidLibraryUsage, "Passed in an invalid MemReqs object." };
}

Texas::ResultValue<Texas::FileInfo> Texas::detail::PrivateAccessor::parseStream(InputStream& stream) noexcept
{
    Result result{};

    // Load the identifier. 12 bytes is the largest identifer
    // that we know of so far. After that we go back to where
    // we were in the stream because the loaders assume the stream 
    // is at the start of the file.
    std::byte identifier[12] = {};
    std::size_t prevPos = stream.tell();
    result = stream.read({ identifier, 12 });
    if (!result.isSuccessful())
        return result;
    stream.seek(prevPos);

    FileInfo memReqs{};

    // We test the file's identifier to see if it's KTX
    if (std::memcmp(identifier, KTX::identifier, sizeof(KTX::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_KTX_READ
        Result result = KTX::loadFromStream(stream, memReqs.m_textureInfo);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSize(memReqs.textureInfo());
            return { static_cast<FileInfo&&>(memReqs) };
        }
        else
            return { result };
#else
        return { ResultType::FileNotSupported, 
            "Encountered a KTX-file. "
            "KTX support has not been enabled in this configuration." };
#endif
    }

    /*
    // Test if it's a PNG file
    if (std::memcmp(inputBuffer.data(), PNG::identifier, sizeof(PNG::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_PNG_READ
        Result result = PNG::loadFromBuffer_Step1(inputBuffer, memReqs.m_textureInfo, memReqs.m_workingMemoryRequired, memReqs.m_backendData.png);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSize(memReqs.textureInfo());
            return { static_cast<FileInfo&&>(memReqs) };
        }
        else
            return { result };
#else
        return { ResultType::FileNotSupported, "Encountered a PNG-file. PNG support has not been enabled in this configuration." };
#endif
    }
    */

    return { ResultType::FileNotSupported, 
             "Could not identify file-format of input, "
             "or file-format is not supported." };
}

Texas::Result Texas::detail::PrivateAccessor::loadImageData(
    InputStream& stream,
    FileInfo const& file, 
    ByteSpan dstBuffer, 
    ByteSpan workingMem) noexcept
{
    if (dstBuffer.data() == nullptr)
        return { ResultType::InvalidLibraryUsage, "You need to send in a destination buffer." };
    if (dstBuffer.size() < file.memoryRequired())
        return { ResultType::InvalidLibraryUsage, 
                 "Destination buffer is not equal to or higher than Texas::FileInfo::memoryRequired(). "
                 "Cannot fit image data in this buffer." };
    if (file.workingMemoryRequired() > 0)
    {
        if (workingMem.data() == nullptr)
            return { ResultType::InvalidLibraryUsage, 
                     "Cannot pass nullptr for working-memory when loading image-data requires working-memory." };
        else if (workingMem.size() < file.workingMemoryRequired())
            return { ResultType::InvalidLibraryUsage, 
                     "Working-memory passed in is not large enough to load the image-data." };
    }   

#ifdef TEXAS_ENABLE_KTX_READ
    if (file.textureInfo().fileFormat == FileFormat::KTX)
    {
        return detail::KTX::loadImageData(stream, dstBuffer, file.textureInfo());
    }
#endif

    /*
#ifdef TEXAS_ENABLE_PNG_READ
    if (file.textureInfo().fileFormat == FileFormat::PNG)
    {
        return detail::PNG::loadFromBuffer_Step2(file.textureInfo(), file.m_backendData.png, dstBuffer, workingMem);
    }
#endif
    */

    return { ResultType::InvalidLibraryUsage, "Passed in an invalid FileInfo object." };
}

Texas::ResultValue<Texas::Texture> Texas::detail::PrivateAccessor::loadFromStream(
    InputStream& stream, 
    Allocator* allocator) noexcept
{
    ResultValue<FileInfo> parseFileResult = parseStream(stream);
    if (!parseFileResult.isSuccessful())
        return { parseFileResult.resultType(), parseFileResult.errorMessage() };

    FileInfo const& fileInfo = parseFileResult.value();

    Texture returnVal{};
    returnVal.m_textureInfo = fileInfo.textureInfo();
    returnVal.m_allocator = allocator;

    std::uint64_t const dstBufferSize = fileInfo.memoryRequired();

    // Test that the system can hold the size of the image-data.
    if constexpr (detail::maxValue<std::uint64_t>() > detail::maxValue<std::size_t>())
    {
        if (dstBufferSize > detail::maxValue<std::size_t>())
            return { ResultType::FileNotSupported, "Image requires more memory than the system can possibly allocate." };
    }

    // Allocate destination buffer
    if (allocator != nullptr)
    {
        std::byte* buffer = allocator->allocate(
            static_cast<std::size_t>(dstBufferSize),
            Allocator::MemoryType::ImageData);
        returnVal.m_buffer = ByteSpan{ buffer, static_cast<std::size_t>(dstBufferSize) };
        if (returnVal.m_buffer.data() == nullptr)
            return { ResultType::InvalidLibraryUsage, 
            "Allocator returned nullptr when attempting to allocate memory for image-data." };
    }
    else
    {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
        std::byte* buffer = new std::byte[static_cast<std::size_t>(dstBufferSize)];
        returnVal.m_buffer = { buffer, static_cast<std::size_t>(dstBufferSize) };
#else
        // This path should never be reached!
#endif
    }

    // Allocate working memory if needed
    std::byte* workingMem = nullptr;
    std::uint64_t workingMemSize = fileInfo.workingMemoryRequired();
    if constexpr (detail::maxValue<std::uint64_t>() > detail::maxValue<std::size_t>())
    {
        if (workingMemSize > detail::maxValue<std::size_t>())
            return { ResultType::FileNotSupported, 
                     "Texture requires more working memory than the system can possibly allocate." };
    }
    if (workingMemSize > 0)
    {
        if (allocator != nullptr)
        {
            workingMem = allocator->allocate(static_cast<std::size_t>(workingMemSize), Allocator::MemoryType::WorkingData);
            if (workingMem == nullptr)
                return { ResultType::InvalidLibraryUsage, "Allocator returned nullptr when attempting to allocate working-memory." };
        }
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            workingMem = new std::byte[static_cast<std::size_t>(workingMemSize)];
#else
            // This path should never be reached!
#endif
        }
    }

    Result loadResult = loadImageData(stream, fileInfo, returnVal.m_buffer, { workingMem, workingMemSize });
    // Deallocate the working-memory
    if (workingMem != nullptr)
    {
        if (allocator != nullptr)
            allocator->deallocate(workingMem, Allocator::MemoryType::WorkingData);
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            delete[] workingMem;
#else
            // This path should never be reached!
#endif
        }
    }
    if (!loadResult.isSuccessful())
        return loadResult;

    return returnVal;
}

Texas::ResultValue<Texas::Texture> Texas::detail::PrivateAccessor::loadFromBuffer(
    ConstByteSpan inputBuffer, 
    Allocator* allocator) noexcept
{
    ResultValue<FileInfo> parseFileResult = parseBuffer(inputBuffer);
    if (!parseFileResult.isSuccessful())
        return { parseFileResult.resultType(), parseFileResult.errorMessage() };

    FileInfo const& fileInfo = parseFileResult.value();

    Texture returnVal{};
    returnVal.m_textureInfo = fileInfo.textureInfo();
    returnVal.m_allocator = allocator;

    std::uint64_t dstBufferSize = fileInfo.memoryRequired();

    if constexpr (detail::maxValue<std::uint64_t>() > detail::maxValue<std::size_t>())
    {
        if (dstBufferSize > detail::maxValue<std::size_t>())
            return { ResultType::FileNotSupported, "Image requires more memory than the system can possibly allocate." };
    }

    // Allocate destination buffer
    if (allocator != nullptr)
    {
        std::byte* buffer = allocator->allocate(static_cast<std::size_t>(dstBufferSize), Allocator::MemoryType::ImageData);
        returnVal.m_buffer = { buffer, static_cast<std::size_t>(dstBufferSize) };
        if (returnVal.m_buffer.data() == nullptr)
            return { ResultType::InvalidLibraryUsage, "Allocator returned nullptr when attempting to allocate memory for image-data." };
    }
    else
    {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
        returnVal.m_buffer = ByteSpan{ 
            new std::byte[static_cast<std::size_t>(dstBufferSize)],
            static_cast<std::size_t>(dstBufferSize)
        };
#else
        // Disabled for now because this function is supposed to be noexcept.
        //TEXAS_DETAIL_EXCEPTION(false, "Library bug: "
        //                              "Function 'detail::PrivateAccessor::loadFromBuffer' received a nullptr allocator, "
        //                              "but dynamic allocations have not been enabled.");
#endif
    }

    // Allocate working memory if needed
    std::byte* workingMem = nullptr;
    std::uint64_t workingMemSize = fileInfo.workingMemoryRequired();

    if constexpr (detail::maxValue<std::uint64_t>() > detail::maxValue<std::size_t>())
    {
        if (workingMemSize > detail::maxValue<std::size_t>())
            return { ResultType::FileNotSupported, "Image requires more working memory than the system can possibly allocate." };
    }

    if (workingMemSize > 0)
    {
        if (allocator != nullptr)
        {
            workingMem = allocator->allocate(static_cast<std::size_t>(workingMemSize), Allocator::MemoryType::WorkingData);
            if (workingMem == nullptr)
                return { ResultType::InvalidLibraryUsage, "Allocator returned nullptr when attempting to allocate working-memory." };
        }
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            workingMem = new std::byte[static_cast<std::size_t>(workingMemSize)];
#else
        // We already covered this case above.
#endif
        }
    }

    Result loadResult = loadImageData(fileInfo, returnVal.m_buffer, ByteSpan{ workingMem, static_cast<std::size_t>(workingMemSize) });
    
    if (workingMem != nullptr)
    {
        if (allocator != nullptr)
            allocator->deallocate(workingMem, Allocator::MemoryType::WorkingData);
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            delete[] workingMem;
#endif
        }
    }

    if (!loadResult.isSuccessful())
        return { loadResult };

    return returnVal;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
Texas::ResultValue<Texas::Texture> Texas::loadFromStream(InputStream& stream) noexcept
{
    return detail::PrivateAccessor::loadFromStream(stream, nullptr);
}

#include <fstream>
Texas::ResultValue<Texas::Texture> Texas::loadFromPath(char const* path) noexcept
{
    class Test : public InputStream
    {
    public:
        std::ifstream internalStream{};

        virtual Result read(ByteSpan dst) noexcept override
        {
            if (internalStream.eof())
                return { ResultType::PrematureEndOfFile, 
                         "Encountered premate end of file." };
            internalStream.read((char*)dst.data(), dst.size());
            return { ResultType::Success, nullptr };
        }
        virtual void ignore(std::size_t amount) noexcept override
        {
            internalStream.ignore(amount);
        }
        virtual std::size_t tell() noexcept override
        {
            return internalStream.tellg();
        }
        virtual void seek(std::size_t pos) noexcept override
        {
            internalStream.seekg(pos);
        }
    };

    Test temp{};
    temp.internalStream = std::ifstream(path, std::ios::binary);
    return loadFromStream(temp);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(ConstByteSpan inputBuffer) noexcept
{
    return detail::PrivateAccessor::loadFromBuffer(inputBuffer, nullptr);
}
#endif // End ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
