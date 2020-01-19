# Examples
## The easiest way
Currently, the easiest way to load a texture is from a buffer and into the `Texas::Texture` abstraction class. This class represents the entirety of the loaded image, which includes image-data and metadata. The functions for this loading path have the following signatures
```cpp
Texas::ResultValue<Texas::Texture> loadFrombuffer(const std::byte* buffer, std::uint64_t bufferSize);
Texas::ResultValue<Texas::Texture> loadFrombuffer(Texas::ConstByteSpan fileBuffer);
```
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

// The image-data span for the base image, this includes array elements.
// This function can fail, and therefore return a Texas::Optional<ConstByteSpan>
// but since know all images have a mip-level 0 and all textures returned from
// Texas loading functions are valid, we can dereference the value right away.
Texas::ConstByteSpan baseMipLeveImageData = loadTexture.mipSpan(0).value();

// The former function returns a Texas::ConstByteSpan if successful,
// the pointer and size can be acquired separately using the following
const std::byte* ptr = baseMipLevelImageData.data();
unsigned int size = baseMipLevelImageData.size();

// The image-data for the 2nd array layer in mip-level 3
//
// These types of methods may fail 
// - If the mip-level index is equal or higher than the texture's mip-level count, 
// - If the array-layer index is equal or higher than the texture's array-layer count.
// - If the texture-object has become invalid (usually as a result of being moved from) 
Texas::ConstByteSpan wantedArrayLayer = loadTexture.arrayLevelSpan(2, 1).value();
```
The list of Texture's methods can be found in the [Texture.hpp](https://github.com/Didgy74/Texas/blob/development/include/Texas/Texture.hpp) header file.
