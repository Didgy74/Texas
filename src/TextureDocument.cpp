#include "DTex/TextureDocument.hpp"

#include <cmath>

namespace DTex
{
	TextureDocument::TextureDocument(CreateInfo&& in)
	{
		byteArray = std::move(in.byteArray);
		mipMapDataInfo = in.mipMapDataInfo;
		baseDimensions = in.baseDimensions;
		textureType = in.textureType;
		pixelFormat = in.pixelFormat;
		isCompressed = in.isCompressed;
		mipLevels = in.mipLevels;
		arrayLayers = in.arrayLayers;

		assert(mipLevels > 0 && "Error. Must have atleast 1 mipLevel.");
	}

	Dimensions TextureDocument::GetDimensions(uint32_t mipLevel) const
	{
		Dimensions returnValue;
		for (size_t dim = 0; dim < 3; dim++)
			returnValue[dim] = uint32_t(baseDimensions[dim] / (std::pow(2, mipLevel)));
		return returnValue;
	}

	const uint8_t * TextureDocument::GetData(uint32_t mipLevel) const
	{
		return byteArray.data() + mipMapDataInfo[mipLevel].offset;
	}

	uint32_t TextureDocument::GetDataSize(uint32_t mipLevel) const
	{
		return uint32_t(mipMapDataInfo[mipLevel].byteLength);
	}

	const uint8_t* TextureDocument::GetInternalBuffer() const
	{
		return byteArray.data();
	}

	size_t TextureDocument::GetInternalBufferSize() const
	{
		return byteArray.size();
	}

	TextureType TextureDocument::GetTextureType() const
	{
		return textureType;
	}

	PixelFormat TextureDocument::GetPixelFormat() const
	{
		return pixelFormat;
	}

	bool TextureDocument::IsCompressed() const
	{
		return isCompressed;
	}

	uint32_t TextureDocument::GetArrayLayers() const
	{
		return arrayLayers;
	}

	uint32_t TextureDocument::GetMipLevels() const
	{
		return mipLevels;
	}
}