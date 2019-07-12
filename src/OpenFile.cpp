#include "DTex/OpenFile.hpp"


#include "DTex/DTex.hpp"

#include "DTex/Tools.hpp"

std::optional<DTex::Dimensions> DTex::OpenFile::GetDimensions(uint32_t mipLevel) const
{
	if (mipLevel >= metaData.mipLevelCount)
		return {};

	return Tools::CalcMipmapDimensions(metaData.baseDimensions, mipLevel);
}

size_t DTex::OpenFile::GetTotalSizeRequired() const
{
	return Tools::CalcTotalSizeRequired(metaData.baseDimensions, metaData.mipLevelCount, metaData.arrayLayerCount, metaData.pixelFormat);
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