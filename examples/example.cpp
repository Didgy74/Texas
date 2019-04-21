
#include "DTex/DTex.hpp"
#include "DTex/VkFormats.hpp"
#include "DTex/GLFormats.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main()
{
    auto path = "resources/test3.png";

	int x;
	int y;
	int comps;
	auto img = stbi_load(path, &x, &y, &comps, 0);
	

	auto loadResult = DTex::LoadFromFile(path);

	if (loadResult.GetResultInfo() != DTex::ResultInfo::Success)
	{
		std::cout << loadResult.GetErrorMessage() << std::endl;
		std::exit(0);
	}

	auto& texDoc = loadResult.GetValue();

	for (size_t i = 0; i < texDoc.GetDataSize(0); i++)
	{
		if (texDoc.GetInternalBuffer()[i] != img[i])
		{
			std::cout << "NO: " << std::to_string(i) << std::endl;
			std::cout << "STBI: " << std::to_string(img[i]) << std::endl;
			std::cout << "DTex: " << std::to_string(texDoc.GetInternalBuffer()[i]) << std::endl;
			std::exit(0);
		}
	}

	stbi_image_free(img);

	if (loadResult.GetResultInfo() != DTex::ResultInfo::Success)
	{
		std::cout << "Failed to load file '" << path << "'. Terminating.." << std::endl;
		std::exit(0);
	}

	std::cout << "Loaded file '" << path << "' succesfully." << std::endl;
	
	auto baseDimensions = texDoc.GetDimensions(0);
	std::cout << "Base dimensions are " << baseDimensions.width << " x " << baseDimensions.height << " x " << baseDimensions.depth << "." << std::endl;

	if (texDoc.IsCompressed())
		std::cout << "The loaded image uses a compressed pixel format." << std::endl;
	else
		std::cout << "The loaded image uses an uncompressed pixel format." << std::endl;

	std::cout << "TextureDocument's internal buffer contains " << texDoc.GetInternalBufferSize() << " bytes of data." << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "OpenGL info:" << std::endl;

	if (DTex::ToGLFormat(texDoc.GetPixelFormat()) == DTex::ToGLFormat(DTex::PixelFormat::BC7_UNorm))
		std::cout << "Image format is GL_COMPRESSED_RGBA_BPTC_UNORM." << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "Vulkan info:" << std::endl;

	if (DTex::ToVkFormat(texDoc.GetPixelFormat()) == DTex::ToVkFormat(DTex::PixelFormat::BC7_UNorm))
		std::cout << "Format is VK_FORMAT_BC7_UNORM_BLOCK." << std::endl;

	if (DTex::ToVkImageType(texDoc.GetTextureType()) == DTex::ToVkImageType(DTex::TextureType::Texture2D))
		std::cout << "ImageType is VK_IMAGE_TYPE_2D" << std::endl;


	return 0;
}