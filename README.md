# Texas
This library aims to simplify texture loading for C++17 programs, with a focus on (but not limited to) 3D applications. The goal is to have a modular library where most, if not all, functionality is opt-in. This lets people enjoy Texas as a lightweight barebones library, with the opportunity to opt-in to more advanced functionality. 

Texas will include opt-in tools to load the texture to OpenGL and Vulkan.

**NOTE!** This library is very early in it's development stages, interface breaking changes, bugs, features may be added/removed at any time over the course of the early stages of development. This is the first library I'm building that's meant to be used by others, you are warned.

## Requirements
The library has been tested to run on the following compilers
#### Windows x64
 - MSVC v14.21

#### Ubuntu x64
 - Not tested yet

## Installation
Installation is built around CMake, but it is possible to integrate the library without it. By default, Texas does not enable any functionality. To get it to compile, you need to enable reading for **atleast** one file-format. The easiest way is to just enable all features. You can do this and add all the correct files to your project by adding the following lines to your CMakeLists.txt

```cmake
set(TEXAS_ENABLE_EVERYTHING ON)
add_subdirectory("Path to Texas folder")
target_link_libraries("myTargetName" PUBLIC Texas)
```

You can gain access to the library from a source file by using `#include` at the top of your file like so:
```cpp
#include "Texas/Texas.hpp"
```

### CMake options
 - `TEXAS_ENABLE_EVERYTHING` Enables all the functionality Texas has to offer
 - `TEXAS_ENABLE_KTX_READ` Enables loading KTX files
 - `TEXAS_ENABLE_PNG_READ` Enables loading PNG files
 - `TEXAS_ENABLE_VULKAN_TOOLS` Enables tools to load a texture into Vulkan

## Limitations
The current version is capable of loading only KTX and PNG files, from pre-loaded buffers into user-allocated memory. Additional ways of loading, like loading directly from file using std::ifstream and allowing the library to dynamically allocate memory for you, will be added eventually.

PNG files are always decompressed to R, RG, RGB or RGBA upon loading based on the file's pixel format.
PNG support is currently limited to the following features:
 - Grayscale 8-bit per channel
 - RGB 8-bit per channel
 - RGBA 8-bit per channel
 - Non-interlaced

Currently supported KTX features:
 - All uncompressed pixel formats.
 - All BCn formats
 - All DXT formats

1D textures, 3D textures, cubemaps are **not yet** supported. Support for these will be added over time.

ETC and ASTC support is planned.

## Planned features
 - Full support to read and write KTX, PNG, BMP, DDS, KTX2 files
 - LoadFromBuffer loading method with allocation callbacks
 - Loading directly from file
 - Loading into library-allocated memory
 - Library allocations using custom allocator
 - Option for enabling STL -> Texas struct conversions
 - Write up to date examples on how to use with Vulkan and OpenGL
 

### Dependencies

 - zLib 1.2.11 - [zLib Home Site](https://www.zlib.net/)
 I have made small changes to the source code to silence some error messages on MSVC compiler. These changes should not affect runtime behavior.

### Contribution and Feedback
I will very much welcome feedback or any suggestions. Please open issues or pull requests to show me what can be improved.

### Thank you for using my library
Sincerely, Nils Petter Skålerud aka "Didgy"
