# DTex
KTX and KTX2 texture-loading project by Nils Petter Skålerud (Didgy)

This library aims to simplify KTX loading for C++ programs. It includes tools to load the texture to OpenGL and Vulkan.

**NOTE!** This library is very early in it's development stages, interface breaking changes, bugs, features may be added/removed at any time over the course of the early stages of development. This is the first library I'm building that's meant to be used by others, you are warned.

## Requirements
Hopefully the library should work on any C++17 compliant compiler.

The library has been tested to run on the following
#### Windows x64
 - MSVC v14
 - MinGW 7.0

 #### Ubuntu x64
 - GCC 8.2.0

## Installation
You can copy the contents of the `include` folder directly into your project.

Alternatively, you can use CMake. You can add all the correct files to your project by adding the following lines to your CMakeLists.txt

```cmake
add_subdirectory("Path to DTex folder")
target_link_libraries("myTargetName" DTex::DTex)
```

You can gain access to the library from a source file by using `#include` at the top of your file like so:
```cpp
#include "DTex/DTex.hpp"
```
## Examples
The example(s) can be compiled using a compiler of your choice, refer to the "Requirements" section for a list of tested compilers

You can tell CMake to automatically compile the example(s) into their own executables. This can be done by adding the following line to the CMakeLists.txt in the root of the library (not the root of your project.
```cmake
set(COMPILE_EXAMPLES 1)
```
How the start of the CMakeLists.txt should look afterwards:
```cmake
cmake_minimum_required(VERSION 3.12)
project(DTex)
set(COMPILE_EXAMPLES 1)
```
## Limitations
The current version of the library should (hopefully) work on KTX files, containing 2D images, compressed or uncompressed with mipmaps. The limitations of the library over time, as it is currently under testing.

1D textures, 3D textures, texture arrays, cubemaps are **not** supported. Support for these will be added over time.

PNG and JPEG support is planned to be added eventually, but this library will focus on loading KTX and KTX2 textures first and foremost.

Currently supported compression formats:

 - Uncompressed RGB, unsigned normalized
 - Uncompressed RGBA unsigned normalized
 - All BCn formats (needs testing to confirm)
 - All DXT formats (needs testing to confirm)

ETC and ASTC support is planned.
## Example usage with OpenGL
```cpp
#include "DTex/DTex.hpp"
#include "DTex/GLFormats.hpp"

auto texDocOpt = DTex::LoadFromFile("Data/Textures/02.ktx");
if (texDocOpt.has_value() == false)
	std::cout << "Error. Could not load file!" << std::endl;

auto& texDoc = texDocOpt.value();

GLuint textureHandle;

glGenTextures(1, &textureHandle);

GLenum target = DTex::ToGLTarget(texDoc.GetTextureType());

glBindTexture(target, textureHandle);

GLint format = DTex::ToGLFormat(texDoc.GetPixelFormat());
GLenum type = DTex::ToGLType(texDoc.GetPixelFormat());

for (size_t level = 0; level < texDoc.GetMipLevels(); level++)
{
	GLsizei width = texDoc.GetDimensions(level).width;
	GLsizei height = texDoc.GetDimensions(level).height;
	auto data = texDoc.GetData(level);
	GLsizei dataLength = texDoc.GetDataSize(level);

	if (texDoc.IsCompressed())
		glCompressedTexImage2D(target, level, format, width, height, 0, dataLength, data);
	else
		glTexImage2D(target, level, format, width, height, 0, format, type, data);
}

if (texDoc.GetMipLevels() > 1)
{
	glTexParameteri(target, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, texDoc.GetMipLevels() - 1);
}
```
A Vulkan example will be provided eventually. For now you can use `uint32_t DTex::GetVkFormat()` to get the correct `VkFormat`, and `uint32_t DTex::GetVkImageType()` to get the correct `VkImageType`.  These functions can be found in the `VkFormats.hpp` header file. Although tools for simplifying loading into Vulkan have not been made yet, I am 90% certain you will find all the information necessary to do so in this library.

## Contribution and Feedback
I will very much welcome feedback or any suggestions. Please open issues or pull requests to show me what can be improved.

 
### Thank you for using my library
Sincerely, Nils Petter Skålerud aka "Didgy"

