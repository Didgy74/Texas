## Example usage with OpenGL
This example shows how to use DTex with OpenGL with the simplest method, by using `DTex::LoadFromFile`. This loads all metadata and image-data immediately into the `DTex::TextureDocument` class upon success.
```cpp
#include "DTex/DTex.hpp"
#include "DTex/GLFormats.hpp"

std::filesystem::path pathToFile = "test.ktx";

DTex::LoadInfo<DTex::TexDoc> loadInfo = DTex::LoadFromFile(pathToFile);

if (loadInfo.IsSuccessful() == false)
{
	DTex::ResultType errorCode = loadInfo.GetResultType();
	std::string_view errorMessage = loadInfo.GetErrorMessage();
	// Handle error
}

const DTex::TexDoc& texDoc = loadInfo.GetValue();

GLuint textureHandle;

glGenTextures(1, &textureHandle);

glBindTexture(target, textureHandle);

// Grabs the OpenGL data from the DTex::TexDoc structure.
GLenum target = DTex::ToGLTarget(input.GetTextureType());
GLint internalFormat = DTex::ToGLInternalFormat(input.GetPixelFormat(), input.GetColorSpace());
GLenum format = DTex::ToGLFormat(input.GetPixelFormat());
GLenum type = DTex::ToGLType(input.GetPixelFormat());

// Iterates over mipmap-levels and loads all image data onto OpenGL.
for (GLint level = 0; level < GLint(input.GetMipLevelCount()); level++)
{
	GLsizei width = GLsizei(input.GetMipDimensions(level).value().width);
	GLsizei height = GLsizei(input.GetMipDimensions(level).value().height);

	const uint8_t* data = input.GetMipData(level).value();
	GLsizei dataLength = GLsizei(input.GetMipLevelSize(level).value());

	if (input.IsCompressed())
		glCompressedTexImage2D(target, level, internalFormat, width, height, 0, dataLength, data);
	else
		glTexImage2D(target, level, internalFormat, width, height, 0, format, type, data);
}

// Sets mipmapping parameters.
if (input.GetMipLevelCount() > 1)
{
	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, GLint(input.GetMipLevelCount() - 1));
}
else
	glGenerateTextureMipmap(ibo.texture);
```

## Example usage with Vulkan
This example shows how to load imagedata from file directly onto mapped memory in Vulkan, by using `DTex::LoadFromFile_Deferred` and `DTex::LoadImageData`. The process will usually be something like:
1. Load file-metadata from DTex
2. Query the returning struct for the image-details. Space required to hold the image, pixelformat, dimensions, mips etc.
3. Create the VkImage or VkBuffer, allocate the corresponding VkDeviceMemory and map the memory
4. Load the imagedata onto the mapped memory with DTex
```cpp
#include "DTex/DTex.hpp"
#include "DTex/VkFormats.hpp"

std::filesystem::path pathToFile = "test.ktx";

DTex::LoadInfo<DTex::OpenFile> loadInfo = DTex::LoadFromFile(pathToFile);

if (loadInfo.IsSuccessful() == false)
{
	DTex::ResultType errorCode = loadInfo.GetResultType();
	std::string_view errorMessage = loadInfo.GetErrorMessage();
	// Handle error
}

const DTex::OpenFile& openFile = loadInfo.GetValue();

const DTex::Dimensions& fileDim = openFile.GetBaseDimensions();

uint32_t imgArrayLayers = openFile.GetArrayLayerCount();
VkExtent3D imgExtent = VkExtent3D{ fileDim.width, fileDim.height, fileDim.depth };
VkFormat imgFormat = (VkFormat) DTex::ToVkFormat(openFile.GetPixelFormat(), openFile.GetColorSpace());
VkImageType imgType = (vk::ImageType) DTex::ToVkImageType(openFile.GetTextureType());
uint32_t mipLevelCount = openFile.GetMipLevelCount();

// Tells you how much space is needed to fit the entire texture onto memory.
size_t sizeRequired = openFile.GetTotalSizeRequired();

/*
    NOTE!
    Create the VkBuffer or VkImage here using the information provided!
*/

void* mappedMemoryPtr;
VkResult mapResult = vkMapMemory(myDevice, myDeviceMemory, 0, sizeRequired, 0, &mappedMemoryPtr);
assert(mapResult == VK_SUCCESS);

DTex::LoadImageData(openFile, (uint8_t*)mappedMemoryPtr);

// Remember to flush mapped memory, or unmap it
```