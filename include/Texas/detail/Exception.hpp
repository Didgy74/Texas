#pragma once

#undef TEXAS_DETAIL_EXCEPTION

#ifdef TEXAS_ENABLE_EXCEPTIONS

#include <stdexcept>

#define TEXAS_DETAIL_EXCEPTION(expression, msg) \
{ \
	if (!static_cast<bool>(expression)) \
	{ \
		throw msg; \
	} \
} \

#define TEXAS_DETAIL_NOEXCEPT noexcept

#else
	
#define TEXAS_DETAIL_EXCEPTION(expression, msg)

#define TEXAS_DETAIL_NOEXCEPT

#endif