#include "Texas/Texas.hpp"
#include "PrivateAccessor.hpp"

#include <cstring>

#ifdef TEXAS_ENABLE_KTX_READ
#	include "KTX.hpp"
#endif

#ifdef TEXAS_ENABLE_PNG_READ
#	include "PNG.hpp"
#endif

namespace Texas
{
	LoadResult<OpenBuffer> loadFromBuffer(const std::byte* const fileBuffer, const std::size_t bufferLength)
	{
		return detail::PrivateAccessor::loadFromBuffer(ConstByteSpan(fileBuffer, bufferLength));
	}

	LoadResult<OpenBuffer> loadFromBuffer(const ConstByteSpan inputBuffer)
	{
		return detail::PrivateAccessor::loadFromBuffer(inputBuffer);
	}

	Result loadImageData(const OpenBuffer& file, const ByteSpan dstBuffer, const ByteSpan workingMemory)
	{
		return detail::PrivateAccessor::loadImageData(file, dstBuffer, workingMemory);
	}

	Result loadImageData(const OpenBuffer& file, std::byte* const dstBuffer, const std::size_t dstBufferSize, std::byte* const workingMemory, const std::size_t workingMemorySize)
	{
		return detail::PrivateAccessor::loadImageData(file, ByteSpan(dstBuffer, dstBufferSize), ByteSpan(workingMemory, workingMemorySize));
	}
}

namespace Texas
{
	LoadResult<OpenBuffer> detail::PrivateAccessor::loadFromBuffer(const ConstByteSpan inputBuffer)
	{
		// Check if input buffer is larger than 0.
		// 12 bytes is the largest file identifier... So far.
		if (inputBuffer.size() < 12)
			return LoadResult<OpenBuffer>(ResultType::InvalidInputParameter, "Buffer provided cannot have length 0.");

		OpenBuffer openBuffer{};

#ifdef TEXAS_ENABLE_KTX_READ
		// We test the file's identifier to see if it's KTX
		if (std::memcmp(inputBuffer.data(), detail::KTX::Header::correctIdentifier, sizeof(detail::KTX::Header::correctIdentifier)) == 0)
		{
			Result result = detail::KTX::loadFromBuffer_Step1(true, inputBuffer, openBuffer.m_metaData, openBuffer.m_backendData.ktx);
			if (result.resultType() == ResultType::Success)
				return LoadResult<OpenBuffer>(static_cast<OpenBuffer&&>(openBuffer));
			else
				return LoadResult<OpenBuffer>(result.resultType(), result.errorMessage());
		}
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		if (std::memcmp(inputBuffer.data(), detail::PNG::Header::identifier, sizeof(detail::PNG::Header::identifier)) == 0)
		{
			Result result = detail::PNG::loadFromBuffer_Step1(true, inputBuffer, openBuffer.m_metaData, openBuffer.m_backendData.png);
			if (result.resultType() == ResultType::Success)
				return LoadResult<OpenBuffer>(static_cast<OpenBuffer&&>(openBuffer));
			else
				return LoadResult<OpenBuffer>(result.resultType(), result.errorMessage());
		}
#endif

		return LoadResult<OpenBuffer>(ResultType::FileNotSupported, "Could not identify file-format of input, or file-format is not supported.");
	}

	Result detail::PrivateAccessor::loadImageData(const OpenBuffer& file, const ByteSpan dstBuffer, const ByteSpan workingMemory)
	{
		if (dstBuffer.size() < file.memoryRequired())
			return Result(ResultType::InvalidInputParameter, "Destination buffer is not equal or higher than Texas::OpenBuffer::memoryRequired(). Cannot fit image data in this buffer.");
		if (workingMemory.data() != nullptr && workingMemory.size() < file.workingMemoryRequired())
			return Result(ResultType::InvalidInputParameter, "Working memory provided is not large enough for Texas to unpack the image data.");

#ifdef TEXAS_ENABLE_KTX_READ
		if (file.metaData().srcFileFormat == FileFormat::KTX)
		{
			return detail::KTX::loadFromBuffer_Step2(file.metaData(), file.m_backendData.ktx, dstBuffer, workingMemory);
		}
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		if (file.metaData().srcFileFormat == FileFormat::PNG)
		{
			if (workingMemory.size() == 0 || workingMemory.data() == nullptr)
				return { ResultType::InvalidInputParameter, "Cannot decompress PNG with no working memory." };

			return detail::PNG::loadFromBuffer_Step2(file.metaData(), file.m_backendData.png, dstBuffer, workingMemory);
		}
#endif

		return Result(ResultType::InvalidInputParameter, nullptr);
	}
}


