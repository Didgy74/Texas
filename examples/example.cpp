#include "DTex/DTex.hpp"

#include <iostream>
#include <vector>
 
int main()
{
	auto path = "resources/02.ktx";

	DTex::LoadInfo<DTex::TextureDocument> loadInfo = DTex::LoadFromFile(path);

	if (loadInfo.IsSuccessful() == false)
	{
		std::cout << DTex::ToString(loadInfo.GetResultType()) << std::endl;
		std::cout << loadInfo.GetErrorMessage() << std::endl;
		return 0;
	}

	DTex::TextureDocument& texDoc = loadInfo.GetValue();

	std::cout << "Loaded image file was of type " << DTex::ToString(texDoc.GetSourceFileFormat()) << std::endl;

	std::cout << "TextureDocument's internal buffer contains " << texDoc.GetTotalSizeRequired() << " bytes of data." << std::endl;

	const DTex::Dimensions baseDimensions = texDoc.GetBaseDimensions();
	std::cout << "Base dimensions are " << baseDimensions.width << " x " << baseDimensions.height << " x " << baseDimensions.depth << "." << std::endl;

	if (texDoc.IsCompressed())
		std::cout << "The loaded image uses a compressed pixel format." << std::endl;
	else
		std::cout << "The loaded image uses an uncompressed pixel format." << std::endl;

	std::cout << "The loaded image has pixelformat: " << DTex::ToString(texDoc.GetPixelFormat()) << std::endl;

	std::cout << "The loaded image is in encoded in colorspace " << DTex::ToString(texDoc.GetColorSpace()) << std::endl;

	std::cout << "Amount of mip levels: " << texDoc.GetMipLevelCount() << std::endl;

	std::cout << "Amount of array layers: " << texDoc.GetArrayLayerCount() << std::endl;
		
	return 0;
}