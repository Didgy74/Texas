#include "KTX.hpp"


#include "DTex/DTex.hpp"
#include "DTex/GLFormats.hpp"
#include "DTex/Tools.hpp"

#include "DebugConfig.hpp"
#include "PrivateAccessor.hpp"

#include <cstring>
#include <string_view>

namespace DTex::detail::KTX
{
	static constexpr TextureType ToTextureType(const Header::Member_T* dimensions, const Header::Member_T arrayLayerCount)
	{
		if (dimensions[2] == Header::Member_T(0))
		{
			if (dimensions[1] == Header::Member_T(0))
				return TextureType::Texture1D;
			else
				return TextureType::Texture2D;
		}
		else
			return TextureType::Texture3D;
	}
}

bool DTex::detail::KTX::LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultInfo& resultInfo, std::string_view& errorMessage)
{
	metaData.srcFileFormat = FileFormat::KTX;

	using namespace std::literals;

	uint8_t headerBuffer[Header::totalSize];
	fstream.read(reinterpret_cast<char*>(headerBuffer), sizeof(headerBuffer));

	// Check for end of file while reading header
	if (fstream.eof())
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "Reached end-of-file while reading file header."sv;
		return false;
	}

	// Check for correct identifier
	const Header::Identifier_T* const fileIdentifier = reinterpret_cast<const Header::Identifier_T*>(headerBuffer + Header::identifierOffset);
	if (std::memcmp(fileIdentifier, Header::correctIdentifier, sizeof(Header::Identifier_T)) != 0)
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "File identifier does not match KTX identifier."sv;
		return false;
	}

	// Check if file endianness matches system's
	const Header::Member_T& fileEndian = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::endianOffset);
	if (fileEndian != Header::correctEndian)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "File endianness does not match system endianness. DTex not capable of converting."sv;
		return false;
	}

	// Load dimension data
	const Header::Member_T originalDimensions[3]
	{
		*reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelWidthOffset),
		*reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelHeightOffset),
		*reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelDepthOffset)
	};
	if (originalDimensions[0] == 0)
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "KTX specification does not allow width to be 0."sv;
		return false;
	}
	
	const Header::Member_T originalArrayCount = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::numberOfArrayElementsOffset);

	metaData.textureType = ToTextureType(originalDimensions, metaData.arrayLayerCount);

	metaData.baseDimensions.width = originalDimensions[0];
	metaData.baseDimensions.height = originalDimensions[1];
	if (metaData.baseDimensions.height == 0)
		metaData.baseDimensions.height = 1;
	metaData.baseDimensions.depth = originalDimensions[2];
	if (metaData.baseDimensions.depth == 0)
		metaData.baseDimensions.depth = 1;

	metaData.arrayLayerCount = originalArrayCount;
	if (metaData.arrayLayerCount == 0)
		metaData.arrayLayerCount = 1;

	metaData.mipLevelCount = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::numberOfMipmapLevelsOffset);
	if (metaData.mipLevelCount == 0)
		metaData.mipLevelCount = 1;

	// Grab pixel format
	const Header::Member_T& fileGLInternalFormat = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::glInternalFormatOffset);

	const Header::Member_T& fileGLType = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::glTypeOffset);

	auto [pixelFormat, colorSpace] = ToPixelFormat(fileGLInternalFormat, fileGLType);
	metaData.pixelFormat = pixelFormat;
	metaData.colorSpace = colorSpace;
	if (metaData.pixelFormat == PixelFormat::Invalid || metaData.colorSpace == ColorSpace::Invalid)
	{
		resultInfo = ResultInfo::PixelFormatNotSupported;
		errorMessage = "KTX pixel format not supported."sv;
		return false;
	}

	// Read key value data (Right now we ignore it)
	const Header::Member_T& fileKeyValueDataLength = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::bytesOfKeyValueDataOffset);
	if (fileKeyValueDataLength != 0)
		fstream.ignore(fileKeyValueDataLength);

	if constexpr (detail::enableExtendedDebug)
	{
		std::streampos imageDataStart = fstream.tellg();

		fstream.seekg(fstream.end);

		std::streampos imageDataEnd = fstream.tellg();

		auto test = imageDataEnd - imageDataStart;

		fstream.seekg(imageDataStart);
	}

	return true;
}

bool DTex::detail::PrivateAccessor::KTX_LoadImageData(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer)
{
	for (uint32_t mip = 0; mip < metaData.mipLevelCount; mip++)
	{
		KTX::Header::Member_T imageSize = 0;

		fstream.read(reinterpret_cast<char*>(&imageSize), sizeof(imageSize));

		if constexpr (detail::enableExtendedDebug)
		{
			size_t correctImageSize = Tools::CalcImageDataSize_Unsafe(Tools::CalcMipmapDimensions(metaData.baseDimensions, mip), metaData.pixelFormat);
			if (correctImageSize <= std::numeric_limits<KTX::Header::Member_T>::max())
			{
				// Safely can cast to uint32_t
				if ((uint32_t)correctImageSize != imageSize)
					// Error. Imagesize is not correct.
					return false;
			}
			else
				// Image size is too large for KTX
				return false;
		}

		fstream.read(reinterpret_cast<char*>(dstBuffer), imageSize);

		if constexpr (detail::enableExtendedDebug)
		{
			if ((mip < metaData.mipLevelCount - 1) && fstream.eof())
				// Reached premature end-of-file when loading image-data
				return false;
		}

		size_t mipPadding = imageSize % KTX::Header::Member_T(4);
		if (mipPadding != 0)
			fstream.ignore(mipPadding);

		dstBuffer += imageSize;
	}

	return true;
}