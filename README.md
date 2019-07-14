# DTex
KTX and KTX2 texture-loading project by Nils Petter Skålerud (Didgy)

This library aims to simplify texture loading for C++ programs, with a focus on (but not limited to) 3D applications. It includes tools to load the texture to OpenGL and Vulkan.

**NOTE!** This library is very early in it's development stages, interface breaking changes, bugs, features may be added/removed at any time over the course of the early stages of development. This is the first library I'm building that's meant to be used by others, you are warned.

## Requirements
The library should work on any C++17 compliant compiler.

The library has been tested to run on the following compilers
#### Windows x64
 - MSVC v14.21

 #### Ubuntu x64
 - GCC 9.1.0

## Installation
You can copy the contents of the `include` and `src` folder directly into your project. Remember to add all source-files in `src` as targets for compilation.

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
The current version is capable of loading KTX and PNG files. It will auto-detect the format based on the path provided.

PNG files are always decompressed to R, RG, RGB or RGBA upon loading based on the file's pixel format.
PNG support is currently limited to the following formats
 - Grayscale 8-bit per channel
 - RGB 8-bit per channel
 - RGBA 8-bit per channel
 - Non-interlaced

1D textures, 3D textures, texture arrays, cubemaps are **not** supported. Support for these will be added over time.

Currently supported pixel-formats:
 - Uncompressed R, unsigned normalized, 8-bit
 - Uncompressed RG, unsigned normalized, 8-bit
 - Uncompressed RGB, unsigned normalized, 8-bit
 - Uncompressed RGBA unsigned normalized, 8-bit
 - All BCn formats
 - All DXT formats

ETC and ASTC support is planned.

### Dependencies

 - zLib 1.2.11 - [zLib Home Site](https://www.zlib.net/)
 I have made small changes to the source code to silence some error messages on MSVC compiler. These changes should not affect runtime behavior.

### Contribution and Feedback
I will very much welcome feedback or any suggestions. Please open issues or pull requests to show me what can be improved.

### Thank you for using my library
Sincerely, Nils Petter Skålerud aka "Didgy"


