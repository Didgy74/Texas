# Examples
## The easiest way
Currently, the easiest way to load a texture is from a buffer and into the `Texas::Texture` abstraction class. This class represents the entirety of the loaded image, which includes image-data and metadata. The functions for this loading path have the following signatures
```cpp
ResultValue<Texture> loadFrombuffer(const std::byte* buffer, std::uint64_t bufferSize);
ResultValue<Texture> loadFrombuffer(ConstByteSpan fileBuffer);
```
The struct `ReturnValue<T>` is a combination of the `Result` struct and an optional value of type `T`. In case of an error, you can query it for the error code and error message. If successful, it will contain a valid value of `T`.

These functions are made available only when the CMake option `TEXAS_ENABLE_DYNAMIC_ALLOCATIONS` is enabled.
```cpp
#include "Texas/Texas.hpp"

unsigned int bufferForFileDataSize = // Size of your loaded buffer
const std::byte* bufferForFileData = // Load your buffer filled with file data

Texas::ConstByteSpan fileBufferSpan = { bufferForFileData, bufferForFileDataSize };

Texas::ResultValue<Texas::Texture> loadResult = Texas::loadFromBuffer(fileBufferSpan);
if (loadResult.isSuccessful() == false)
{
    Texas::ResultType errorCode = loadResult.resultType();
    const char* errorMessage = loadResult.errorMessage();
    // Handle error
}

Texas::Texture& loadedTexture = loadResult.value();
// Alternatively you can move the value
// by doing Texas::Texture loadedTexture = std::move(loadResult.value());
```
Now that you've loaded the texture, you can gain access to the image data by using the Texture methods
```cpp
Texas::PixelFormat pixelFormat = loadedTexture.pixelFormat();

Texas::Dimensions baseDimensions = loadTexture.baseDimensions();

// The image-data span for the base image, this includes array elements.'
// Passing in an index equal or higher than loadTexture.mipLevelCount() is UB.
Texas::ConstByteSpan baseMipLeveImageData = loadTexture.mipSpan(0);

// The former function returns a Texas::ConstByteSpan if successful,
// the pointer and size can be acquired separately using the following
const std::byte* ptr = baseMipLevelImageData.data();
unsigned int size = baseMipLevelImageData.size();

// The image-data for the 2nd array layer in mip-level 3
//
// These types of methods may fail 
// - If the mip-level index is equal or higher than the texture's mip-level count, 
// - If the array-layer index is equal or higher than the texture's array-layer count.
// - If the texture-object is not containing any data (usually happens only when moving data out of a Texture)
Texas::ConstByteSpan wantedArrayLayer = loadTexture.arrayLevelSpan(2, 1);
```
The list of Texture's methods can be found in the [Texture.hpp](https://github.com/Didgy74/Texas/blob/master/include/Texas/Texture.hpp) header file.

## Loading into your buffer
The process of loading image-data onto your own pre-allocated buffer is usually three steps:
 1. Parse file and get memory requirements
 2. Allocate memory for the image-data
	 - If the loader requires some working-memory in order to unpack the image-data, you will also need to allocate this memory additionally.
3. Load image-data onto the allocated memory

In terms of code, you will be using one of the following functions
```cpp
ResultValue<FileInfo> parseBuffer(const std::byte* inputBuffer, std::size_t bufferSize) noexcept;
ResultValue<FileInfo> parseBuffer(ConstByteSpan inputBuffer) noexcept;
```
If successful, the function returns a struct `FileInfo` which contains the amount of image-memory required, amount of working-memory required and the texture-info of the file. It also contains some hidden data used for speeding up loading image-data from the input-buffer later.

After allocating the data needed, you will want to use one of the following functions
```cpp
Result loadImageData(FileInfo const& file, ByteSpan dstBuffer, ByteSpan workingMemory) noexcept;
Result loadImageData(ParsedFileInfo const& file, std::byte* dstBuffer, std::size_t dstBufferSize, std::byte* workingMemory, std::size_t workingMemorySize) noexcept;
```
If `FileInfo::workingMemoryRequired()` returns 0, you can pass in `nullptr` and 0 for the working memory parameters.

The struct `Result` contains a field for an error code of type `ResultType`, and also a `const char*` for an error message.

The function requires that there has been no changes to the `inputBuffer`  passed into `parseBuffer` in between these function calls. Any changes to the input file-buffer will make calling loadImageData undefined behavior, it might return an error-code if it can catch the error.

If this function is successful, you will find all the image-data in `dstBuffer` and can refer to the `parseBuffer` struct for querying offset for mip-levels, array-layers.

Code example:
```cpp
#include "Texas/Texas.hpp"

unsigned int bufferForFileDataSize = // Size of your loaded buffer
const std::byte* bufferForFileData = // Load your buffer filled with file data

Texas::ConstByteSpan fileBufferSpan = { bufferForFileData, bufferForFileDataSize };

Texas::ResultValue<Texas::FileInfo> parseFileResult = Texas::parseBuffer(fileBufferSpan);
if (!parseFileResult.isSuccessful())
{
    Texas::ResultType errorCode = parseFileResult.resultType();
    const char* errorMessage = parseFileResult.errorMessage();
    // Handle error
}

Texas::TextureInfo textureInfo = parseFileResult.value();

unsigned int dstBufferSize = textureInfo.memoryRequired();
std::byte* dstBuffer = // Allocate your destination buffer
Texas::ByteSpan dstBufferSpan = { dstBuffer, dstBufferSize };

unsigned int workingMemSize = 0;
std::byte* workingMem = nullptr;
if (textureInfo.workingMemoryRequired() > 0)
{
    workingMemSize = memoryRequirements.workingMemoryRequired();
    workingMem = Allocate the working memory.
}
Texas::ByteSpan workingMemSpan = { workingMem, workingMemSize };

Texas::Result loadImageDataResult = Texas::loadImageData(memoryRequirements, dstBufferSpan, workingMemSpan);
if (!loadImageDataResult.isSuccessful())
{
    Texas::ResultType errorCode = loadImageDataResult.resultType();
    const char* errorMessage = loadImageDataResult.errorMessage();
    // Handle error
}
```
