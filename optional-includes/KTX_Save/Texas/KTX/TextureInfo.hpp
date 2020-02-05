#pragma once

#include "Texas/KTX/TextureType.hpp"

#include "Texas/TextureInfo.hpp"
#include "Texas/ResultValue.hpp"

#include <cstdint>

namespace Texas::KTX
{
	/*
	struct TextureInfo
	{
		TextureType textureType = TextureType::Invalid;
		std::uint32_t width = 0;
		std::uint32_t height = 0;
		std::uint32_t depth = 0;
		std::uint32_t arrayLayerCount = 0;
		std::uint32_t mipLevelCount = 0;
	};

	[[nodiscard]] ResultValue<KTX::TextureInfo> toTextureInfo(Texas::TextureInfo const& texInfo) noexcept;
	*/
}