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
	LoadResult<OpenBuffer> loadFromBuffer_Deferred(const void* fileBuffer, std::size_t bufferLength)
	{
		return detail::PrivateAccessor::loadFromBuffer_Deferred(fileBuffer, bufferLength);
	}

	Result LoadImageData(const OpenBuffer& file, std::uint8_t* const dstBuffer, std::uint8_t* const workingMemory)
	{
		return detail::PrivateAccessor::LoadImageData(file, dstBuffer, workingMemory);
	}
}

namespace Texas
{
	LoadResult<OpenBuffer> detail::PrivateAccessor::loadFromBuffer_Deferred(const void* fileBuffer, std::size_t bufferLength)
	{
		if (bufferLength == 0)
			return LoadResult<OpenBuffer>(ResultType::InvalidInputParameter, "Argument 'bufferLength' in Texas::loadFromBuffer_Deferred cannot be 0.");

		OpenBuffer openBuffer{};

		Span<const std::uint8_t> span = Span(static_cast<const std::uint8_t*>(fileBuffer), bufferLength);

#ifdef TEXAS_ENABLE_KTX_READ
		if (std::memcmp(fileBuffer, detail::KTX::Header::correctIdentifier, sizeof(detail::KTX::Header::correctIdentifier)) == 0)
		{
			Pair<ResultType, const char*> result = detail::KTX::loadTest(true, span, openBuffer.m_metaData, openBuffer.imageDataStart);
			if (result.a() == ResultType::Success)
				return LoadResult<OpenBuffer>(static_cast<OpenBuffer&&>(openBuffer));
			else
				return LoadResult<OpenBuffer>(result.a(), result.b());
		}
#endif

		return LoadResult<OpenBuffer>(ResultType::FileNotSupported, "Could not identify file-format of input, or file-format is not supported.");
	}

	Result detail::PrivateAccessor::LoadImageData(const OpenBuffer& file, std::uint8_t* const dstBuffer, std::uint8_t* const workingMemory)
	{
#ifdef TEXAS_ENABLE_KTX_READ
		if (file.metaData().srcFileFormat == FileFormat::KTX)
		{
			return detail::KTX::loadImageDataFromBuffer(file.metaData(), file.imageDataStart, dstBuffer);
		}
#endif

#ifdef TEXAS_ENABLE_PNG_READ
		// If workingMemory is nullptr, make error
#endif

		return Result(ResultType::Success, nullptr);
	}
}


