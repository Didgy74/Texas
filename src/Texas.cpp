// Stops the warnings made by MSVC when using "unsafe" CRT fopen functions.
#ifdef _MSC_VER
#   define _CRT_SECURE_NO_WARNINGS
#endif

#include <Texas/Texas.hpp>
#include "PrivateAccessor.hpp"
#include <Texas/Tools.hpp>
#include "NumericLimits.hpp"

#include "KTX.hpp"
#include "PNG.hpp"

#include <cstring>
// For file IO
#include <cstdio>

#if !defined(TEXAS_ENABLE_KTX_READ) && !defined(TEXAS_ENABLE_PNG_READ)
#error Cannot compile Texas without enabling atleast one file-format.
#endif

namespace Texas::detail
{
	class FileIOStreamWrapper : public InputStream
	{
	public:
		std::FILE* filestream = nullptr;
		virtual ~FileIOStreamWrapper()
		{
			if (filestream != nullptr)
			{
				std::fclose(filestream);
			}
		}
		virtual Result read(ByteSpan dst) noexcept override
		{
			std::size_t bytesRead = std::fread(dst.data(), 1, dst.size(), filestream);
			if (bytesRead != dst.size())
				return { ResultType::PrematureEndOfFile, "Reached premature end of filestream." };
			return { ResultType::Success, nullptr };
		}
		virtual void ignore(std::size_t amount) noexcept override
		{
			int seekResult = std::fseek(filestream, static_cast<long>(amount), SEEK_CUR);
			if (seekResult != 0)
			{
				// TODO: handle seek error
			}
		}
		virtual std::size_t tell() noexcept override
		{
			long tellResult = std::ftell(filestream);
			if (tellResult == -1)
			{
				// TODO: Error
			}
			return static_cast<std::size_t>(tellResult);
		}
		virtual void seek(std::size_t pos) noexcept override
		{
			int seekResult = std::fseek(filestream, static_cast<long>(pos), SEEK_SET);
			if (seekResult != 0)
			{
				// TODO: handle seek error
			}
		}
	};

	class BufferStreamWrapper : public InputStream
	{
	public:
		ConstByteSpan inputBuffer{};
		std::size_t offset = 0;

		virtual Result read(ByteSpan dst) noexcept override
		{
			if (offset + dst.size() > inputBuffer.size())
				return { ResultType::PrematureEndOfFile, "Encountered premature end of file when loading." };
			std::memcpy(dst.data(), inputBuffer.data() + offset, dst.size());
			offset += dst.size();
			return { ResultType::Success, nullptr };
		}
		virtual void ignore(std::size_t amount) noexcept override
		{
			offset += amount;
		}
		virtual std::size_t tell() noexcept override
		{
			return offset;
		}
		virtual void seek(std::size_t pos) noexcept override
		{
			offset = pos;
		}
	};
}

Texas::ResultValue<Texas::Texture> Texas::loadFromStream(InputStream& stream, Allocator& allocator) noexcept
{
	return detail::PrivateAccessor::loadFromStream(stream, &allocator);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromPath(char const* path, Allocator& allocator) noexcept
{
	detail::FileIOStreamWrapper temp{};

	temp.filestream = std::fopen(path, "rb");
	if (temp.filestream == nullptr)
		return { ResultType::CouldNotOpenFile, "Failed to open this file for reading." };
	return loadFromStream(temp, allocator);
}

Texas::ResultValue<Texas::FileInfo> Texas::parseStream(InputStream& stream) noexcept
{
	return detail::PrivateAccessor::parseStream(stream);
}

Texas::Result Texas::loadImageData(
	InputStream& stream,
	FileInfo const& file,
	ByteSpan dstBuffer,
	ByteSpan workingMemory) noexcept
{
	return detail::PrivateAccessor::loadImageData(stream, file, dstBuffer, workingMemory);
}

#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
Texas::ResultValue<Texas::Texture> Texas::loadFromStream(InputStream& stream) noexcept
{
	return detail::PrivateAccessor::loadFromStream(stream, nullptr);
}

Texas::ResultValue<Texas::Texture> Texas::loadFromPath(char const* path) noexcept
{
	detail::FileIOStreamWrapper temp{};
	temp.filestream = std::fopen(path, "rb");
	if (temp.filestream == nullptr)
		return { ResultType::CouldNotOpenFile, "Failed to open this file for reading." };
	return loadFromStream(temp);
}
#endif // End ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS

Texas::ResultValue<Texas::FileInfo> Texas::detail::PrivateAccessor::parseStream(InputStream& stream) noexcept
{
	Result result{};

	// Load the identifierBuffer. 12 bytes is the largest identifer
	// that we know of so far. After that we go back to where
	// we were in the stream because the loaders assume the stream 
	// is at the start of the file.
	std::byte identifierBuffer[12] = {};
	std::size_t prevPos = stream.tell();
	result = stream.read({ identifierBuffer, 12 });
	if (!result.isSuccessful())
		return result;
	stream.seek(prevPos);

	FileInfo memReqs{};

	// Test identifier for KTX
	if (std::memcmp(identifierBuffer, KTX::identifier, sizeof(KTX::identifier)) == 0)
	{
#ifdef TEXAS_ENABLE_KTX_READ
		Result result = KTX::loadFromStream(stream, memReqs.m_textureInfo);
		if (result.isSuccessful())
		{
			memReqs.m_memoryRequired = calculateTotalSize(memReqs.textureInfo());
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


	// Test identifier for PNG
	if (std::memcmp(identifierBuffer, PNG::identifier, sizeof(PNG::identifier)) == 0)
	{
#ifdef TEXAS_ENABLE_PNG_READ
		Result result = PNG::parseStream(
			stream,
			memReqs.m_textureInfo,
			memReqs.m_workingMemoryRequired,
			memReqs.m_backendData.png);
		if (result.isSuccessful())
		{
			memReqs.m_memoryRequired = calculateTotalSize(memReqs.textureInfo());
			return { static_cast<FileInfo&&>(memReqs) };
		}
		else
			return { result };
#else
		return { ResultType::FileNotSupported, "Encountered a PNG-file. "
						 "PNG support has not been enabled in this configuration." };
#endif
	}

	return { ResultType::FileNotSupported,
					 "Could not identify file-format of input "
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
		return detail::KTX::loadImageData(
			stream,
			dstBuffer,
			file.textureInfo(),
			file.m_backendData.ktx);
	}
#endif
#ifdef TEXAS_ENABLE_PNG_READ
	if (file.textureInfo().fileFormat == FileFormat::PNG)
	{
		return detail::PNG::loadFromStream(
			stream,
			file.textureInfo(),
			file.m_backendData.png,
			dstBuffer,
			workingMem);
	}
#endif

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

	Result loadResult = loadImageData(
		stream,
		fileInfo,
		returnVal.m_buffer,
		{ workingMem, static_cast<std::size_t>(workingMemSize) });
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
