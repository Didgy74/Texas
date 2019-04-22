
#include "DTex/DTex.hpp"
#include "DTex/VkFormats.hpp"
#include "DTex/GLFormats.hpp"

#include <chrono>
#include <iostream>

int main()
{
	auto path = "resources/test.png";

	auto now = std::chrono::high_resolution_clock::now();
	auto loadResult = DTex::LoadFromFile(path);
	auto now2 = std::chrono::high_resolution_clock::now();
	auto duration = now2 - now;
	std::cout << "Time taken to load in seconds: " << duration.count() / double(1000) / 1000 / 1000 << std::endl;

	if (loadResult.GetResultInfo() != DTex::ResultInfo::Success)
	{
		std::cout << "Failed to load file '" << path << std::endl;
		std::cout << "Detailed error: " << loadResult.GetErrorMessage() << std::endl;
		std::cout << "Terminating..." << std::endl;
		std::exit(0);
	}

	std::cout << "Loaded file '" << path << "' succesfully." << std::endl;
	
	auto& texDoc = loadResult.GetValue();

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
	else if (DTex::ToGLFormat(texDoc.GetPixelFormat()) == DTex::ToGLFormat(DTex::PixelFormat::R8G8B8_UNorm))
		std::cout << "Image format is GL_RGB. glType is GL_UNSIGNED_BYTE." << std::endl;
	else if (DTex::ToGLFormat(texDoc.GetPixelFormat()) == DTex::ToGLFormat(DTex::PixelFormat::R8G8B8A8_UNorm))
		std::cout << "Image format is GL_RGBA. glType is GL_UNSIGNED_BYTE" << std::endl;

	if (DTex::ToGLTarget(texDoc.GetTextureType()) == DTex::ToGLTarget(DTex::TextureType::Texture2D))
		std::cout << "Texture target type is GL_TEXTURE_2D" << std::endl;

	std::cout << std::endl << std::endl;
	std::cout << "Vulkan info:" << std::endl;

	if (DTex::ToVkFormat(texDoc.GetPixelFormat()) == DTex::ToVkFormat(DTex::PixelFormat::BC7_UNorm))
		std::cout << "Format is VK_FORMAT_BC7_UNORM_BLOCK." << std::endl;
	else if (DTex::ToVkFormat(texDoc.GetPixelFormat()) == DTex::ToVkFormat(DTex::PixelFormat::R8G8B8_UNorm))
		std::cout << "Image format is VK_FORMAT_R8G8B8_UNORM." << std::endl;
	else if (DTex::ToVkFormat(texDoc.GetPixelFormat()) == DTex::ToVkFormat(DTex::PixelFormat::R8G8B8A8_UNorm))
		std::cout << "Image format is VK_FORMAT_R8G8B8_UNORM." << std::endl;

	if (DTex::ToVkImageType(texDoc.GetTextureType()) == DTex::ToVkImageType(DTex::TextureType::Texture2D))
		std::cout << "ImageType is VK_IMAGE_TYPE_2D" << std::endl;


	return 0;
}