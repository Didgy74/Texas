# Texas
v0.1

Texas is a library that aims to simplify texture loading for C++17 programs, with a focus on 3D applications. The goal is to have a library where most, if not all, functionality is modular and can be opted out. This allows users to configure Texas to match their project's requirements and be as lightweight as possible.

Texas also includes opt-in tools to load textures into OpenGL and Vulkan.

**NOTE!** This library is very early in it's development stages, interface breaking changes, bugs, features may appear/disappear at any time over the course of the early stages of development. This is the first library I'm building that's meant to be used by others, you are warned. I currently do not recommend using Texas in it's current state if you need stability, but I'm very happy if you would use it and give me feedback and bug reports.

# Getting started
## For instructions on how to integrate and use Texas, head over to the Wiki pages.

## Requirements
The library has been tested to run on the following compilers
#### Windows x64
 - MSVC 14.25
#### Linux x64
 - GCC 9.2.1
 - Clang 9.0.1


## Limitations
Texas is currently very limited in terms of functionality and supported files. So far it can only load basic KTX and PNG files, and save basic KTX files. Texas does not support modifying textures in any way, and will only load the texture as it exists in the file.

Support for handling color-space data is still very limited in KTX.

    
### PNG
PNG files are always decompressed to a grayscale, grayscale with alpha, RGB or RGBA 2D image upon loading based on the file's pixel format. PNG support is currently limited to the following features:
 - Grayscale
 - Grayscale with alpha
 - RGB 8-bit per channel
 - RGBA 8-bit per channel
 - Indexed colors

Texas does not yet support interlaced PNG images.

### KTX
KTX files will have all their mipmaps and array-layers extracted. Texas will not modify the image-data in any way. Currently tested KTX formats:
 - R 8-bit
 - RGB 8-bit 
 - RGBA 8-bit
 - BC7
 - ASTC

Texas does not yet support cubemaps, 1D textures or 3D textures. Nor does it yet support ETC and ASTC compressed textures.
ETC and ASTC support is planned.

## Planned features
 - Full support to read formats:
	 - KTX
	 - PNG
	 - DDS
	 - KTX2
	 - BMP
 - 1D textures
 - 3D textures
 - Cubemaps
 - ETC - Read only
 - Color space information
 

### Dependencies
 - zLib 1.2.11 - [zLib Home Site](https://www.zlib.net/)
	 - zLib gets linked when you enable PNG support, otherwise it's not compiled at all.

### Contribution and Feedback
Feedback is very much appreciated.
Open an issue if you have a feature request. Open one even if it's already on the list of planned features, that way it's easier to gauge how to prioritize development.

Sincerely, Nils Petter Skålerud aka "Didgy"


