
#define DTEX_IMPLEMENTATION
#include "DTex/DTex.hpp"

#include <iostream>

int main()
{
	auto path = "resources/02.ktx";
	auto texDocOpt = DTex::LoadFromFile(path);

	if (!texDocOpt.has_value())
	{
		std::cout << "Failed to load file '" << path << "'. Terminating.." << std::endl;
		std::exit(0);
	}

	auto& texDoc = texDocOpt.value();

	std::cout << "Loaded file '" << path << "' succesfully." << std::endl;
	
	auto baseDimensions = texDoc.GetDimensions(0);
	std::cout << "Base dimensions are " << baseDimensions[0] << " x " << baseDimensions[1] << " x " << baseDimensions[2] << "." << std::endl;

	if (texDoc.IsCompressed())
		std::cout << "The loaded image uses a compressed pixel format." << std::endl;
	else
		std::cout << "The loaded image uses an uncompressed pixel format." << std::endl;

	std::cout << "TextureDocument's internal buffer contains " << texDoc.GetInternalBufferSize() << " bytes of data." << std::endl;

	if (texDoc.GetGLFormat() == DTex::detail::ToGLFormat(DTex::Format::BC7_UNorm))
		std::cout << "TextureDocument's OpenGL image format is GL_COMPRESSED_RGBA_BPTC_UNORM." << std::endl;

	if (texDoc.GetVkFormat() == DTex::detail::ToVkFormat(DTex::Format::BC7_UNorm))
		std::cout << "TextureDocument's Vulkan format is VK_FORMAT_BC7_UNORM_BLOCK." << std::endl;


	return 0;
}