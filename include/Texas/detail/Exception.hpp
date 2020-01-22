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

#else
	
#define TEXAS_DETAIL_EXCEPTION(expression, msg)

#endif