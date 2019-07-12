#include "DTex/DTex.hpp"

#include "KTX.hpp"
#include "PNG.hpp"
#include "PrivateAccessor.hpp"

#include <fstream>
#include <filesystem>

#include "DTex/TextureDocument.hpp"

DTex::LoadResult<DTex::TextureDocument> DTex::LoadFromFile(std::filesystem::path path)
{
	return detail::PrivateAccessor::LoadFromFile(path);
}

DTex::LoadResult<DTex::TextureDocument> DTex::detail::PrivateAccessor::LoadFromFile(const std::filesystem::path& path)
{
	TextureDocument texDoc;

	std::ifstream filestream = std::ifstream(path, std::ios::binary);
	if (!filestream.is_open())
		return LoadResult<TextureDocument>(ResultInfo::CouldNotReadFile, "Error. Could not open image file.");

	if (path.has_extension())
	{
		const auto& extension = path.extension();
		std::string errorMessage;
		ResultInfo resultInfo;

		if (extension == ".ktx")
		{
			bool result = KTX::LoadHeader_Backend(texDoc.metaData, filestream, resultInfo, errorMessage);
			if (!result)
				return LoadResult<TextureDocument>(resultInfo, std::move(errorMessage));
		}
		else if (extension == ".png")
		{
			bool result = PNG::LoadHeader_Backend(texDoc.metaData, filestream, resultInfo, errorMessage);
			if (!result)
				return LoadResult<TextureDocument>(resultInfo, std::move(errorMessage));
		}
		else
			return LoadResult<TextureDocument>(ResultInfo::FileNotSupported, "File has unknown/unsupported extension");
	}
	else
		return LoadResult<TextureDocument>(ResultInfo::CouldNotReadFile, "File has no extension.");

	texDoc.byteArray.resize(texDoc.metaData.GetTotalSizeRequired());

	switch (texDoc.metaData.srcFileFormat)
	{
	case FileFormat::KTX:
		KTX_LoadImageData_CustomBuffer(filestream, texDoc.metaData, texDoc.byteArray.data());
		break;
	case FileFormat::PNG:
		PNG_LoadImageData_CustomBuffer(filestream, texDoc.metaData, texDoc.byteArray.data());
		break;
	}

	return LoadResult<TextureDocument>(std::move(texDoc));
}

DTex::LoadResult<DTex::OpenFile> DTex::LoadFromFile_CustomBuffer(std::filesystem::path path)
{
	return detail::PrivateAccessor::LoadFile_CustomBuffer(path);
}

DTex::LoadResult<DTex::OpenFile> DTex::detail::PrivateAccessor::LoadFile_CustomBuffer(const std::filesystem::path& path)
{
	OpenFile openFile;

	openFile.filestream = std::ifstream(path, std::ios::binary);
	if (!openFile.filestream.is_open())
		return LoadResult<OpenFile>(ResultInfo::CouldNotReadFile, "Error. Could not open image file.");

	if (path.has_extension())
	{
		const auto& extension = path.extension();
		std::string errorMessage;
		ResultInfo resultInfo;

		if (extension == ".ktx")
		{
			bool result = KTX::LoadHeader_Backend(openFile.metaData, openFile.filestream, resultInfo, errorMessage);
			if (!result)
				return LoadResult<OpenFile>(resultInfo, std::move(errorMessage));
		}
		else if (extension == ".png")
		{
			bool result = PNG::LoadHeader_Backend(openFile.metaData, openFile.filestream, resultInfo, errorMessage);
			if (!result)
				return LoadResult<OpenFile>(resultInfo, std::move(errorMessage));
		}
		else
			return LoadResult<OpenFile>(ResultInfo::FileNotSupported, "File has unknown/unsupported extension");
	}
	else
		return LoadResult<OpenFile>(ResultInfo::CouldNotReadFile, "Loader limitation: Can't load files without an extension.");

	return LoadResult<OpenFile>(std::move(openFile));
}

void DTex::LoadFromOpenFile(const OpenFile& file, uint8_t* dstBuffer)
{
	return detail::PrivateAccessor::LoadFromOpenFile(file, dstBuffer);
}

void DTex::detail::PrivateAccessor::LoadFromOpenFile(const OpenFile& file, uint8_t* dstBuffer)
{
	if (file.metaData.srcFileFormat == FileFormat::KTX)
		detail::PrivateAccessor::KTX_LoadImageData_CustomBuffer(file.filestream, file.metaData, dstBuffer);
	else if (file.metaData.srcFileFormat == FileFormat::PNG)
		detail::PrivateAccessor::PNG_LoadImageData_CustomBuffer(file.filestream, file.metaData, dstBuffer);

	file.filestream.close();
}


