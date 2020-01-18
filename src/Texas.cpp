#include "Texas/Texas.hpp"
#include "PrivateAccessor.hpp"
#include "Texas/detail/Assert.hpp"
#include "Texas/Tools.hpp"

#include <cstring>

#include "KTX.hpp"
#include "PNG.hpp"

Texas::ResultValue<Texas::MemReqs> Texas::getMemReqs(const std::byte* const fileBuffer, const std::size_t bufferLength) noexcept
{
    return detail::PrivateAccessor::getMemReqs(ConstByteSpan(fileBuffer, bufferLength));
}

Texas::ResultValue<Texas::MemReqs> Texas::getMemReqs(const ConstByteSpan inputBuffer) noexcept
{
    return detail::PrivateAccessor::getMemReqs(inputBuffer);
}

Texas::ResultValue<Texas::MetaData> Texas::loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept
{
    return detail::PrivateAccessor::loadImageData(inputBuffer, dstBuffer, workingMemory);
}

Texas::Result Texas::loadImageData(const MemReqs& file, const ByteSpan dstBuffer, const ByteSpan workingMemory) noexcept
{
    return detail::PrivateAccessor::loadImageData(file, dstBuffer, workingMemory);
}

Texas::Result Texas::loadImageData(const MemReqs& file, std::byte* const dstBuffer, const std::size_t dstBufferSize, std::byte* const workingMemory, const std::size_t workingMemorySize) noexcept
{
    return detail::PrivateAccessor::loadImageData(file, ByteSpan(dstBuffer, dstBufferSize), ByteSpan(workingMemory, workingMemorySize));
}

Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(const std::byte* inputBuffer, std::size_t inputBufferSize, Allocator& allocator) noexcept
{
    return detail::PrivateAccessor::loadFromBuffer(ConstByteSpan(inputBuffer, inputBufferSize), &allocator);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(ConstByteSpan inputBuffer, Allocator& allocator) noexcept
{
    return detail::PrivateAccessor::loadFromBuffer(inputBuffer, &allocator);
}

Texas::ResultValue<Texas::MemReqs> Texas::detail::PrivateAccessor::getMemReqs(const ConstByteSpan inputBuffer) noexcept
{
    // Check if input buffer is larger than 0.
    // 12 bytes is the largest file identifier we know of... So far.
    // TODO: Find out how small a file could possibly be and test if it's atleast that size.
    if (inputBuffer.size() < 60)
        return { ResultType::InvalidLibraryUsage, "Input buffer cannot be smaller than 60. "
                                                  "No file supported by Texas can be smaller than 60(?) bytes and still be valid." };

    MemReqs memReqs{};

    // We test the file's identifier to see if it's KTX
    if (std::memcmp(inputBuffer.data(), KTX::identifier, sizeof(KTX::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_KTX_READ
        Result result = KTX::loadFromBuffer_Step1(true, inputBuffer, memReqs.m_metaData, memReqs.m_backendData.ktx);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSizeRequired(memReqs.metaData());
            TEXAS_DETAIL_ASSERT_MSG(memReqs.m_memoryRequired != 0, "Texas author error. A successfully loaded texture cannot have memoryRequired == 0.");
            return { static_cast<MemReqs&&>(memReqs) };
        }
        else
            return { result };
#else
        return { ResultType::FileNotSupported, "Encountered a KTX-file. KTX support has not been enabled in this configuration." };
#endif
    }

    // Test if it's a PNG file
    if (std::memcmp(inputBuffer.data(), PNG::identifier, sizeof(PNG::identifier)) == 0)
    {
#ifdef TEXAS_ENABLE_PNG_READ
        Result result = PNG::loadFromBuffer_Step1(true, inputBuffer, memReqs.m_metaData, memReqs.m_workingMemoryRequired, memReqs.m_backendData.png);
        if (result.isSuccessful())
        {
            memReqs.m_memoryRequired = calcTotalSizeRequired(memReqs.metaData());
            TEXAS_DETAIL_ASSERT_MSG(memReqs.m_memoryRequired != 0, "Texas author error. A successfully loaded texture cannot have memoryRequired == 0.");
            return { static_cast<MemReqs&&>(memReqs) };
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

Texas::Result Texas::detail::PrivateAccessor::loadImageData(const MemReqs& file, const ByteSpan dstBuffer, const ByteSpan workingMem) noexcept
{
    if (dstBuffer.data() == nullptr)
        return { ResultType::InvalidLibraryUsage, "You need to send in a destination buffer." };
    if (dstBuffer.size() < file.memoryRequired())
        return { ResultType::InvalidLibraryUsage, "Destination buffer is not equal to or higher than Texas::MemReqs::memoryRequired(). "
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
    if (file.metaData().srcFileFormat == FileFormat::KTX)
    {
        return detail::KTX::loadFromBuffer_Step2(file.metaData(), file.m_backendData.ktx, dstBuffer, workingMem);
    }
#endif

#ifdef TEXAS_ENABLE_PNG_READ
    if (file.metaData().srcFileFormat == FileFormat::PNG)
    {
        return detail::PNG::loadFromBuffer_Step2(file.metaData(), file.m_backendData.png, dstBuffer, workingMem);
    }
#endif

    return { ResultType::InvalidLibraryUsage, "Passed in an invalid MemReqs object." };
}

Texas::ResultValue<Texas::MetaData> Texas::detail::PrivateAccessor::loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMem) noexcept
{
    if (dstBuffer.data() == nullptr || dstBuffer.size() == 0)
        return { ResultType::InvalidLibraryUsage, "Destination buffer cannot be nullptr or have size 0." };

    ResultValue<MemReqs> parseResult = getMemReqs(inputBuffer);
    if (!parseResult.isSuccessful())
        return { parseResult.resultType(), parseResult.errorMessage() };

    MemReqs memReqs = parseResult.value();
    if (dstBuffer.size() < memReqs.memoryRequired())
        return { ResultType::InvalidLibraryUsage, "Destination buffer not large enough for imagedata." };
    if (memReqs.workingMemoryRequired() > 0 && (workingMem.data() == nullptr || workingMem.size() < memReqs.workingMemoryRequired()))
        return { ResultType::InvalidLibraryUsage, "Not enough working memory." };

    Result loadResult = loadImageData(memReqs, dstBuffer, workingMem);
    if (!loadResult.isSuccessful())
        return { loadResult };

    return { ResultType::FileNotSupported, "Could not identify file-format of input, "
                                           "or file-format is not supported." };
}

Texas::ResultValue<Texas::Texture> Texas::detail::PrivateAccessor::loadFromBuffer(ConstByteSpan inputBuffer, Allocator* allocator) noexcept
{
    ResultValue<MemReqs> parseFileResult = getMemReqs(inputBuffer);
    if (!parseFileResult.isSuccessful())
        return { parseFileResult.resultType(), parseFileResult.errorMessage() };

    MemReqs& memReqs = parseFileResult.value();

    Texture returnVal{};
    returnVal.m_metaData = memReqs.metaData();
    returnVal.m_allocator = allocator;

    // Allocate destination buffer
    auto dstBufferSize = memReqs.memoryRequired();
    if (allocator != nullptr)
    {
        returnVal.m_buffer = ByteSpan{ allocator->allocate(dstBufferSize), dstBufferSize };
        if (returnVal.m_buffer.data() == nullptr)
            return { ResultType::InvalidLibraryUsage, "Allocator returned nullptr when attempting to allocate memory for image-data." };
    }
    else
    {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
        returnVal.m_buffer = ByteSpan{ new std::byte[dstBufferSize], dstBufferSize };
#else
        TEXAS_DETAIL_ASSERT_MSG(false, "Author error. "
                                       "Function 'detail::PrivateAccessor::loadFromBuffer' received a nullptr allocator, "
                                       "but dynamic allocations have not been enabled.");
#endif
    }

    // Allocate working memory if needed
    std::byte* workingMem = nullptr;
    auto workingMemSize = memReqs.workingMemoryRequired();
    if (workingMemSize > 0)
    {
        if (allocator != nullptr)
        {
            workingMem = allocator->allocate(workingMemSize);
            if (workingMem == nullptr)
                return { ResultType::InvalidLibraryUsage, "Allocator returned nullptr when attempting to allocate working-memory." };
        }
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            workingMem = new std::byte[workingMemSize];
#endif
        }
    }

    Result loadResult = loadImageData(memReqs, returnVal.m_buffer, ByteSpan{ workingMem, workingMemSize });
    
    if (workingMem != nullptr)
    {
        if (allocator != nullptr)
            allocator->deallocate(workingMem);
        else
        {
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
            workingMem = new std::byte[workingMemSize];
#endif
        }
    }

    if (!loadResult.isSuccessful())
        return { loadResult };

    return returnVal;
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(const std::byte* fileBuffer, std::size_t bufferLength)
{
    return detail::PrivateAccessor::loadFromBuffer(ConstByteSpan(fileBuffer, bufferLength), nullptr);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromBuffer(ConstByteSpan inputBuffer)
{
    return detail::PrivateAccessor::loadFromBuffer(inputBuffer, nullptr);
}
#endif // End ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
