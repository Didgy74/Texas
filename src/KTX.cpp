#include "KTX.hpp"

#include "DTex/DTex.hpp"
#include "DTex/GLFormats.hpp"
#include "PrivateAccessor.hpp"
#include "DTex/Tools.hpp"

bool DTex::detail::KTX::LoadHeader_Backend(MetaData& metaData, std::ifstream& fstream, ResultInfo& resultInfo, std::string& errorMessage)
{
	uint8_t headerBuffer[Header::totalSize];
	fstream.read(reinterpret_cast<char*>(headerBuffer), sizeof(headerBuffer));

	// Check for end of file while reading header
	if (fstream.eof())
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "Reached end-of-file while reading file header.";
		return false;
	}

	metaData.srcFileFormat = FileFormat::KTX;

	// Check for correct identifier
	const Header::Identifier_T& fileIdentifier = *reinterpret_cast<const Header::Identifier_T*>(headerBuffer + Header::identifierOffset);
	if (std::memcmp(fileIdentifier, Header::correctIdentifier, sizeof(Header::Identifier_T)) != 0)
	{
		resultInfo = ResultInfo::CorruptFileData;
		errorMessage = "File identifier does not match KTX identifier.";
		return false;
	}

	// Check if file endianness matches system's
	const Header::Member_T& fileEndian = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::endianOffset);
	if (fileEndian != Header::correctEndian)
	{
		resultInfo = ResultInfo::FileNotSupported;
		errorMessage = "Loader limitation: File endianness does not match system endianness. Loader is not capable of converting.";
		return false;
	}

	// Load dimension data
	metaData.baseDimensions.width = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelWidthOffset);
	if (metaData.baseDimensions.width <= 0)
		metaData.baseDimensions.width = 1;
	metaData.baseDimensions.height = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelHeightOffset);
	if (metaData.baseDimensions.height <= 0)
		metaData.baseDimensions.height = 1;
	metaData.baseDimensions.depth = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::pixelDepthOffset);
	if (metaData.baseDimensions.depth <= 0)
		metaData.baseDimensions.depth = 1;

	metaData.arrayLayerCount = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::numberOfArrayElementsOffset);
	if (metaData.arrayLayerCount <= 0)
		metaData.arrayLayerCount = 1;

	metaData.mipLevelCount = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::numberOfMipmapLevelsOffset);
	if (metaData.mipLevelCount <= 0)
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
		errorMessage = "KTX pixel format not supported.";
		return false;
	}

	// Read key value data (Right now we ignore it)
	const Header::Member_T& fileKeyValueDataLength = *reinterpret_cast<Header::Member_T*>(headerBuffer + Header::bytesOfKeyValueDataOffset);
	fstream.ignore(fileKeyValueDataLength);

	return true;
}

void DTex::detail::PrivateAccessor::KTX_LoadImageData_CustomBuffer(std::ifstream& fstream, const MetaData& metaData, uint8_t* dstBuffer)
{
	for (size_t i = 0; i < metaData.mipLevelCount; i++)
	{
		KTX::Header::Member_T imageSize;

		fstream.read(reinterpret_cast<char*>(&imageSize), sizeof(imageSize));

		fstream.read(reinterpret_cast<char*>(dstBuffer), imageSize);

		dstBuffer += imageSize;
	}
}