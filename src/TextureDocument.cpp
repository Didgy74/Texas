#include "DTex/TextureDocument.hpp"

#include <cmath>

uint32_t DTex::TextureDocument::GetArrayLayerCount() const
{
	return metaData.arrayLayerCount;
}

const DTex::Dimensions& DTex::TextureDocument::GetBaseDimensions() const
{
	return metaData.baseDimensions;
}

DTex::ColorSpace DTex::TextureDocument::GetColorSpace() const
{
	return metaData.colorSpace;
}

std::optional<const uint8_t*> DTex::TextureDocument::GetMipData(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return byteArray.data() + Tools::CalcTotalSizeRequired_Unsafe(metaData.baseDimensions, mipLevel, metaData.arrayLayerCount, metaData.pixelFormat);
}

const uint8_t* DTex::TextureDocument::GetInternalBufferData() const
{
	return byteArray.data();
}

std::optional<DTex::Dimensions> DTex::TextureDocument::GetMipDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel);
}

uint32_t DTex::TextureDocument::GetMipLevelCount() const
{
	return metaData.mipLevelCount;
}

std::optional<size_t> DTex::TextureDocument::GetMipLevelOffset(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, mipLevel, metaData.arrayLayerCount, metaData.pixelFormat);
}

std::optional<size_t> DTex::TextureDocument::GetMipLevelSize(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcImageDataSize(Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel), metaData.pixelFormat);
}

DTex::PixelFormat DTex::TextureDocument::GetPixelFormat() const
{
	return metaData.pixelFormat;
}

DTex::FileFormat DTex::TextureDocument::GetSourceFileFormat() const
{
	return metaData.srcFileFormat;
}

DTex::TextureType DTex::TextureDocument::GetTextureType() const
{
	return Tools::ToTextureType(metaData.baseDimensions, metaData.arrayLayerCount);
}

size_t DTex::TextureDocument::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, metaData.mipLevelCount, metaData.arrayLayerCount, metaData.pixelFormat);
}

bool DTex::TextureDocument::IsCompressed() const
{
	return Tools::IsCompressed(metaData.pixelFormat);
}