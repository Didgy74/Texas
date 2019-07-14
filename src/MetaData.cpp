#include "DTex/MetaData.hpp"

#include "DTex/Tools.hpp"

std::optional<DTex::Dimensions> DTex::MetaData::GetMipDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(baseDimensions, mipLevel);
}

std::optional<size_t> DTex::MetaData::GetMipLevelOffset(uint32_t mipLevel) const
{
	if (mipLevel >= mipLevelCount)
		return {};

	return Tools::CalcTotalSizeRequired(baseDimensions, mipLevel, arrayLayerCount, pixelFormat);
}

std::optional<size_t> DTex::MetaData::GetMipLevelSize(uint32_t mipLevel) const
{
	if (mipLevel >= mipLevelCount)
		return {};

	return Tools::CalcImageDataSize(Tools::CalcMipmapDimensions(baseDimensions, mipLevel), pixelFormat);
}

size_t DTex::MetaData::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(baseDimensions, mipLevelCount, arrayLayerCount, pixelFormat);
}

bool DTex::MetaData::IsCompressed() const
{
	return Tools::IsCompressed(pixelFormat);
}