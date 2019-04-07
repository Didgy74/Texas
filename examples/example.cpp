
#define DTEX_IMPLEMENTATION
#include "DTex/DTex.hpp"

#include <iostream>

int main()
{
	auto test = DTex::LoadFromFile("04.ktx");

	auto& yo = test.value();

	auto halla = yo.GetGLFormat();

	auto compressed = yo.IsCompressed();

	return 0;
}