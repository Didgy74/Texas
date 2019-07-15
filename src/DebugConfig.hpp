#pragma once

namespace DTex::detail
{
#if defined( NDEBUG )
	constexpr bool enableExtendedDebug = false;
#else
	constexpr bool enableExtendedDebug = true;
#endif
}