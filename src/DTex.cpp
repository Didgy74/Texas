#include "DTex/DTex.hpp"

#include "KTX.hpp"
#include "PNG.hpp"
#include "PrivateAccessor.hpp"
#include "DTex/TextureDocument.hpp"

#include <fstream>
#include <filesystem>
#include <string_view>

DTex::LoadInfo<DTex::TextureDocument> DTex::LoadFromFile(const std::filesystem::path& path)
{
	return detail::PrivateAccessor::LoadFromFile(path);
}

DTex::LoadInfo<DTex::TextureDocument> DTex::detail::PrivateAccessor::LoadFromFile(const std::filesystem::path& path)
{
	using namespace std::literals;

	TextureDocument texDoc;

	std::ifstream filestream = std::ifstream(path, std::ios::binary);
	if (!filestream.is_open())
		return LoadInfo<TextureDocument>(ResultInfo::CouldNotReadFile, "Error. Could not open image file."sv);

	if (path.has_extension())
	{
		const auto& extension = path.extension();
		std::string_view errorMessage;
		ResultInfo resultInfo;

		if (extension == ".ktx")
		{
			bool result = KTX::LoadHeader_Backend(texDoc.metaData, filestream, resultInfo, errorMessage);
			if (!result)
				return LoadInfo<TextureDocument>(resultInfo, std::move(errorMessage));
		}
		else if (extension == ".png")
		{
			bool result = PNG::LoadHeader_Backend(texDoc.metaData, filestream, resultInfo, errorMessage);
			if (!result)
				return LoadInfo<TextureDocument>(resultInfo, std::move(errorMessage));
		}
		else
			return LoadInfo<TextureDocument>(ResultInfo::FileNotSupported, "File has unknown/unsupported extension"sv);
	}
	else
		return LoadInfo<TextureDocument>(ResultInfo::CouldNotReadFile, "File has no extension."sv);

	texDoc.byteArray.resize(texDoc.metaData.GetTotalSizeRequired());

	bool loadImageDataSuccessful = LoadImageData(texDoc.metaData, filestream, texDoc.byteArray.data());
	if (loadImageDataSuccessful == false)
		return LoadInfo<TextureDocument>(ResultInfo::CorruptFileData, "File has corrupted imagedata."sv);

	return LoadInfo<TextureDocument>(std::move(texDoc));
}

DTex::LoadInfo<DTex::OpenFile> DTex::LoadFromFile_Deferred(const std::filesystem::path& path)
{
	return detail::PrivateAccessor::LoadFromFile_Deferred(path);
}

DTex::LoadInfo<DTex::OpenFile> DTex::detail::PrivateAccessor::LoadFromFile_Deferred(const std::filesystem::path& path)
{
	using namespace std::literals;

	OpenFile openFile;

	openFile.filestream = std::ifstream(path, std::ios::binary);
	if (!openFile.filestream.is_open())
		return LoadInfo<OpenFile>(ResultInfo::CouldNotReadFile, "Error. Could not open image file."sv);

	if (path.has_extension())
	{
		const auto& extension = path.extension();
		ResultInfo resultInfo;
		std::string_view errorMessage;

		if (extension == ".ktx")
		{
			bool result = KTX::LoadHeader_Backend(openFile.metaData, openFile.filestream, resultInfo, errorMessage);
			if (!result)
				return LoadInfo<OpenFile>(resultInfo, std::move(errorMessage));
		}
		else if (extension == ".png")
		{
			bool result = PNG::LoadHeader_Backend(openFile.metaData, openFile.filestream, resultInfo, errorMessage);
			if (!result)
				return LoadInfo<OpenFile>(resultInfo, std::move(errorMessage));
		}
		else
			return LoadInfo<OpenFile>(ResultInfo::FileNotSupported, "File has unknown/unsupported extension"sv);
	}
	else
		return LoadInfo<OpenFile>(ResultInfo::CouldNotReadFile, "Loader limitation: Can't load files without an extension."sv);

	return LoadInfo<OpenFile>(std::move(openFile));
}

void DTex::LoadImageData(const OpenFile& file, uint8_t* const dstBuffer)
{
	detail::PrivateAccessor::LoadImageData(file, dstBuffer);
}

bool DTex::detail::PrivateAccessor::LoadImageData(const OpenFile& openFile, uint8_t* const dstBuffer)
{
	return LoadImageData(openFile.metaData, openFile.filestream, dstBuffer);
}

bool DTex::detail::PrivateAccessor::LoadImageData(const MetaData& metaData, std::ifstream& fstream, uint8_t* const dstBuffer)
{
	bool loadImageDataSuccessful = false;
	switch (metaData.srcFileFormat)
	{
	case FileFormat::KTX:
		loadImageDataSuccessful = detail::PrivateAccessor::KTX_LoadImageData(fstream, metaData, dstBuffer);
		break;
	case FileFormat::PNG:
		loadImageDataSuccessful = detail::PrivateAccessor::PNG_LoadImageData(fstream, metaData, dstBuffer);
		break;
	}

	fstream.close();

	return loadImageDataSuccessful;
}


