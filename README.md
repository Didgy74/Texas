# Texas
Version: 0.1

Texas is a library that aims to simplify texture loading for C++17 programs, with a focus on 3D applications. The goal is to have a modular library where most, if not all, functionality is opt-in. This allows Texas to be used as a lightweight, barebones library with the opportunity to opt-in to more advanced functionality. 

Texas also includes opt-in tools to load the texture onto OpenGL and Vulkan renderers.

**NOTE!** This library is very early in it's development stages, interface breaking changes, bugs, features may appear/disappear at any time over the course of the early stages of development. This is the first library I'm building that's meant to be used by others, you are warned. I currently do not recommend using Texas in it's current state if you need stability, but I'm very happy if you would use it and give me feedback and bug reports.

## Requirements
The library has been tested to run on the following compilers
#### Windows x64
 - MSVC 14.24

#### Ubuntu x64
 - GCC 9.1.0

## Integration
Integration is built around CMake, but it is possible to integrate the library without it. By default, Texas does not enable any functionality. To make it to compile, you need to enable reading for **atleast** one file-format. The easiest way to integrate is just enable all features. You can do this and add all the correct files to your project by adding the following lines to your CMakeLists.txt

```cmake
set(TEXAS_ENABLE_EVERYTHING ON)
add_subdirectory("Path to Texas folder")
target_link_libraries("myTargetName" PUBLIC Texas)
```

You gain access to the library from a source file by using `#include` at the top of your file like so:
```cpp
#include "Texas/Texas.hpp"
```
All of the library exists within the `Texas` namespace.
### CMake options
 `TEXAS_ENABLE_EVERYTHING`
 Enables all the functionality Texas has to offer excluding tools for loading into 3D APIs.
 
 `TEXAS_ENABLE_KTX_READ`
 Enables reading KTX files.
 
 `TEXAS_ENABLE_PNG_READ`
 Enables reading PNG files 

`TEXAS_ENABLE_EXCEPTIONS`
Enables exceptions inside the codebase. This can be helpful to catch invalid usage of the interface.

`TEXAS_ENABLE_DYNAMIC_ALLOCATIONS`
Enables the loading paths that lets the implementation do dynamic allocations under the hood

`TEXAS_ENABLE_VULKAN_TOOLS`
Enables tools to load a texture into Vulkan

## Limitations
This version is very limited in functionality and supported files. It can only load KTX and PNG from existing memory buffers.

PNG files are always decompressed to grayscale, grayscale with alpha, RGB or RGBA upon loading based on the file's pixel format. PNG support is currently limited to the following features:
 - Grayscale with alpha
 - RGB 8-bit per channel
 - RGBA 8-bit per channel
 - Indexed colors

Texas does not yet support interlaced PNG images. Support for sRGB colorspace is still pretty bad for PNGs.

The KTX support can load all mipmaps and all array-layers of a texture. It can also extract the color-space information (currently very limited).
Currently tested KTX formats:
 - R 8-bit
 - RGB 8-bit 
 - RGBA 8-bit
 - BC7

Texas does not yet support cubemaps, 1D textures, 3D textures

ETC and ASTC support is planned.

## Planned features
  - Full support to read and write:
	 - KTX
	 - KTX2
	 - DDS
	 - PNG
	 - BMP
 - 1D textures
 - 3D textures
 - Cubemaps
 - Any PixelFormat with channel count 1-4
 - ETC - Read only
 - ASTC - Read only
 - BCn - Read only
 - Color space information
 - Pixel formats with bit-depth higher than 8 bits per channel
 - "Load-from-buffer" loading path with allocation callbacks
 - Loading directly from file by supplying a path
 - Option for enabling STL support, stuff like implicit conversions to equivalent Texas struct.
 - Write up to date examples on how to use with Vulkan and OpenGL
 

### Dependencies

 - zLib 1.2.11 - [zLib Home Site](https://www.zlib.net/)
	 - zLib gets linked when you enable PNG support, otherwise it's not compiled at all.

### Contribution and Feedback
I will very much welcome feedback or any suggestions. Please open issues or pull requests to show me what can be improved.

Sincerely, Nils Petter Skålerud aka "Didgy"

