#include "DTex/TextureDocument.hpp"

#include <cmath>

std::optional<DTex::Dimensions> DTex::TextureDocument::GetDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel);
}

std::optional<const uint8_t*> DTex::TextureDocument::GetData(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return byteArray.data() + Tools::CalcTotalSizeRequired(metaData.baseDimensions, mipLevel, metaData.arrayLayerCount, metaData.pixelFormat);
}