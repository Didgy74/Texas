#pragma once

#include <cstdint>
#include <array>

namespace DTex
{
	namespace detail
	{
		namespace KTX2
		{
			constexpr std::array<uint8_t, 12> identifier{ 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x32, 0x32, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A };
		}
	}
}