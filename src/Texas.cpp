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
	LoadResult<OpenBuffer> loadFromBuffer(const void* const fileBuffer, const std::size_t bufferLength)
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

	Result loadImageData(const OpenBuffer& file, void* const dstBuffer, const std::size_t dstBufferSize, void* const workingMemory, const std::size_t workingMemorySize)
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
			Pair<ResultType, const char*> result = detail::KTX::loadFromBuffer_Step1(true, inputBuffer, openBuffer.m_metaData, openBuffer.imageDataStart);
			if (result.a() == ResultType::Success)
				return LoadResult<OpenBuffer>(static_cast<OpenBuffer&&>(openBuffer));
			else
				return LoadResult<OpenBuffer>(result.a(), result.b());
		}
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		if (std::memcmp(inputBuffer.data(), detail::PNG::Header::identifier, sizeof(detail::PNG::Header::identifier)) == 0)
		{
			Pair<ResultType, const char*> result = detail::PNG::loadFromBuffer_Step1(true, inputBuffer, openBuffer.m_metaData);
			if (result.a() == ResultType::Success)
				return LoadResult<OpenBuffer>(static_cast<OpenBuffer&&>(openBuffer));
			else
				return LoadResult<OpenBuffer>(result.a(), result.b());
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
			return detail::KTX::loadFromBuffer_Step2(file.metaData(), file.imageDataStart, (std::uint8_t*)dstBuffer.data());
		}
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		// If workingMemory is nullptr, make error
#endif

		return Result(ResultType::Success, nullptr);
	}
}


