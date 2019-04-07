
#define DTEX_IMPLEMENTATION
#include "DTex/TextureDocument.hpp"

#include <iostream>

int main()
{
	auto test = DTex::LoadFromFile("test.ktx");

	auto& yo = test.value();



	return 0;
}