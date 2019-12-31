#include "Texas/OpenFile.hpp"

#include "Texas/Tools.hpp"

uint32_t Texas::OpenFile::GetArrayLayerCount() const
{
	return metaData.arrayLayerCount;
}

const Texas::Dimensions& Texas::OpenFile::GetBaseDimensions() const
{
	return metaData.baseDimensions;
}

Texas::ColorSpace Texas::OpenFile::GetColorSpace() const
{
	return metaData.colorSpace;
}

std::optional<Texas::Dimensions> Texas::OpenFile::GetMipDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel);
}

uint32_t Texas::OpenFile::GetMipLevelCount() const
{
	return metaData.mipLevelCount;
}

std::optional<size_t> Texas::OpenFile::GetMipLevelOffset(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, mipLevel, metaData.arrayLayerCount, metaData.pixelFormat);
}

std::optional<size_t> Texas::OpenFile::GetMipLevelSize(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcImageDataSize(Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel), metaData.pixelFormat);
}

Texas::PixelFormat Texas::OpenFile::GetPixelFormat() const
{
	return metaData.pixelFormat;
}

Texas::FileFormat Texas::OpenFile::GetSourceFileFormat() const
{
	return metaData.srcFileFormat;
}

Texas::TextureType Texas::OpenFile::GetTextureType() const
{
	return metaData.textureType;
}

size_t Texas::OpenFile::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, metaData.mipLevelCount, metaData.arrayLayerCount, metaData.pixelFormat);
}

bool Texas::OpenFile::IsCompressed() const
{
	return Tools::IsCompressed(metaData.pixelFormat);
}