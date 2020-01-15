#pragma once

#undef TEXAS_DETAIL_ASSERT
#undef TEXAS_DETAIL_ASSERT_MSG

#ifndef TEXAS_ENABLE_ASSERT

#	define TEXAS_DETAIL_ASSERT(expression)

#	define TEXAS_DETAIL_ASSERT_MSG(condition, msg)

#else

namespace Texas::detail
{
	// Needs to be named this way to not clash with standard assert.
	void texas_assert(const char* conditionString, const char* file, unsigned long long line, const char* msg);
}

#define TEXAS_DETAIL_ASSERT(expression) \
{ \
	if (!static_cast<bool>(expression)) \
	{ \
		Texas::detail::texas_assert(#expression,__FILE__, __LINE__, nullptr); \
	} \
} \


#define TEXAS_DETAIL_ASSERT_MSG(expression, msg) \
{ \
	if (!static_cast<bool>(expression)) \
	{ \
		Texas::detail::texas_assert(#expression,__FILE__, __LINE__, msg); \
	} \
} \

#endif