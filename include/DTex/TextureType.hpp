#pragma once

#include <cstdint>

namespace DTex
{
	enum class TextureType : uint8_t
	{
		Invalid,
		Texture1D,
		Texture2D,
		Texture3D,
	};
}
