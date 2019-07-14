#include "DTex/OpenFile.hpp"

#include "DTex/Tools.hpp"

uint32_t DTex::OpenFile::GetArrayLayerCount() const
{
	return metaData.arrayLayerCount;
}

const DTex::Dimensions& DTex::OpenFile::GetBaseDimensions() const
{
	return metaData.baseDimensions;
}

DTex::ColorSpace DTex::OpenFile::GetColorSpace() const
{
	return metaData.colorSpace;
}

std::optional<DTex::Dimensions> DTex::OpenFile::GetMipDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel);
}

uint32_t DTex::OpenFile::GetMipLevelCount() const
{
	return metaData.mipLevelCount;
}

std::optional<size_t> DTex::OpenFile::GetMipLevelOffset(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, mipLevel, metaData.arrayLayerCount, metaData.pixelFormat);
}

std::optional<size_t> DTex::OpenFile::GetMipLevelSize(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcImageDataSize(Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel), metaData.pixelFormat);
}

DTex::PixelFormat DTex::OpenFile::GetPixelFormat() const
{
	return metaData.pixelFormat;
}

DTex::FileFormat DTex::OpenFile::GetSourceFileFormat() const
{
	return metaData.srcFileFormat;
}

DTex::TextureType DTex::OpenFile::GetTextureType() const
{
	return Tools::ToTextureType(metaData.baseDimensions, metaData.arrayLayerCount);
}

size_t DTex::OpenFile::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, metaData.mipLevelCount, metaData.arrayLayerCount, metaData.pixelFormat);
}

bool DTex::OpenFile::IsCompressed() const
{
	return Tools::IsCompressed(metaData.pixelFormat);
}