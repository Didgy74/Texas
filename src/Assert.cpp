#ifdef TEXAS_ENABLE_ASSERT

#include "Texas/detail/Assert.hpp"

#include <iostream>
#include <cstdlib>

namespace Texas::detail
{
	void texas_assert(const char* conditionString, const char* file, unsigned long long line, const char* msg)
	{
		std::cout << std::endl;
		std::cout << "#######################" << std::endl;
		std::cout << std::endl;
		std::cout << "Texas assertion failure" << std::endl;
		std::cout << std::endl;
		std::cout << "#######################" << std::endl;
		std::cout << "Expression: " << conditionString << std::endl;
		std::cout << "File: " << file << std::endl;
		std::cout << "Line: " << line << std::endl;
		if (msg != nullptr)
			std::cout << "Info: '" << msg << "'" << std::endl;
		std::cout << std::endl;

		std::abort();
	}
}

#endif