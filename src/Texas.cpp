#include "Texas/Texas.hpp"
#include "PrivateAccessor.hpp"
#include "Texas/Tools.hpp"

#include <cstring>

#include "KTX.hpp"
#include "PNG.hpp"

namespace Texas
{
    LoadResult<MemReqs> getMemReqs(const std::byte* const fileBuffer, const std::size_t bufferLength)
    {
        return detail::PrivateAccessor::getMemReqs(ConstByteSpan(fileBuffer, bufferLength));
    }

    LoadResult<MemReqs> getMemReqs(const ConstByteSpan inputBuffer)
    {
        return detail::PrivateAccessor::getMemReqs(inputBuffer);
    }

    LoadResult<MetaData> loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept
    {
        return LoadResult<MetaData>{ ResultType::Success, nullptr };
    }

    Result loadImageData(const MemReqs& file, const ByteSpan dstBuffer, const ByteSpan workingMemory)
    {
        return detail::PrivateAccessor::loadImageData(file, dstBuffer, workingMemory);
    }

    Result loadImageData(const MemReqs& file, std::byte* const dstBuffer, const std::size_t dstBufferSize, std::byte* const workingMemory, const std::size_t workingMemorySize)
    {
        return detail::PrivateAccessor::loadImageData(file, ByteSpan(dstBuffer, dstBufferSize), ByteSpan(workingMemory, workingMemorySize));
    }
}

namespace Texas::detail
{
    LoadResult<MemReqs> PrivateAccessor::getMemReqs(const ConstByteSpan inputBuffer)
    {
        // Check if input buffer is larger than 0.
        // 12 bytes is the largest file identifier we know of... So far.
        // TODO: Find out how small a file could possibly be and test if it's atleast that size.
        if (inputBuffer.size() < 60)
            return { ResultType::InvalidInputParameter, "Buffer provided cannot be smaller than 60. "
                "No file supported by Texas can be smaller than 60(?) bytes and still be valid." };

        MemReqs memReqs{};

        // We test the file's identifier to see if it's KTX
        if (std::memcmp(inputBuffer.data(), KTX::identifier, sizeof(KTX::identifier)) == 0)
        {
#ifdef TEXAS_ENABLE_KTX_READ
            Result result = KTX::loadFromBuffer_Step1(true, inputBuffer, memReqs.m_metaData, memReqs.m_backendData.ktx);
            if (result.isSuccessful())
            {
                memReqs.m_memoryRequired = Tools::calcTotalSizeRequired(memReqs.metaData());
                return LoadResult<MemReqs>(static_cast<MemReqs&&>(memReqs));
            }
            else
                return { result.resultType(), result.errorMessage() };
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
                memReqs.m_memoryRequired = Tools::calcTotalSizeRequired(memReqs.metaData());
                return LoadResult<MemReqs>(static_cast<MemReqs&&>(memReqs));
            }
            else
                return { result.resultType(), result.errorMessage() };
#else
            return { ResultType::FileNotSupported, "Encountered a PNG-file. PNG support has not been enabled in this configuration." };
#endif
        }


        return LoadResult<MemReqs>(ResultType::FileNotSupported, "Could not identify file-format of input, "
                                                                 "or file-format is not supported.");
    }

    Result PrivateAccessor::loadImageData(const MemReqs& file, const ByteSpan dstBuffer, const ByteSpan workingMem)
    {
        if (dstBuffer.data() == nullptr)
            return { ResultType::InvalidInputParameter, "You need to send in a destination buffer." };
        if (dstBuffer.size() < file.memoryRequired())
            return { ResultType::InvalidInputParameter, "Destination buffer is not equal or higher than Texas::MemoryRequirements::memoryRequired(). "
                "Cannot fit image data in this buffer." };
        if (file.workingMemoryRequired() > 0 && (workingMem.size() == 0 || workingMem.data() == nullptr))
            return { ResultType::InvalidInputParameter, "Cannot decompress this file with no working memory." };

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

        return { ResultType::InvalidInputParameter, nullptr };
    }

    LoadResult<MetaData> PrivateAccessor::loadImageData(ConstByteSpan inputBuffer, ByteSpan dstBuffer, ByteSpan workingMem)
    {
        if (dstBuffer.data() == nullptr || dstBuffer.size() == 0)
            return { ResultType::InvalidInputParameter, "No dstBuffer." };

        LoadResult<MemReqs> parseResult = getMemReqs(inputBuffer);
        if (!parseResult.isSuccessful())
            return { parseResult.resultType(), parseResult.errorMessage() };

        MemReqs memReqs = parseResult.value();
        if (dstBuffer.size() < memReqs.memoryRequired())
            return { ResultType::InvalidInputParameter, "Destination buffer not large enough for imagedata." };
        if (memReqs.workingMemoryRequired() > 0 && (workingMem.data() == nullptr || workingMem.size() < memReqs.workingMemoryRequired()))
            return { ResultType::InvalidInputParameter, "Not enough working memory." };

        Result loadResult = loadImageData(memReqs, dstBuffer, workingMem);
        if (!loadResult.isSuccessful())
            return { loadResult.resultType(), loadResult.errorMessage() };

        return LoadResult<MetaData>(ResultType::FileNotSupported, "Could not identify file-format of input, "
            "or file-format is not supported.");
    }
}


