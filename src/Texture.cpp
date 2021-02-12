#include <Texas/Texture.hpp>
#include <Texas/Tools.hpp>

using namespace Texas;

Texture::Texture(Texture&& in) noexcept
{
	m_textureInfo = in.m_textureInfo;
	m_allocator = in.m_allocator;
	m_buffer = in.m_buffer;

	in.m_textureInfo = TextureInfo{};
	in.m_allocator = nullptr;
	in.m_buffer = ByteSpan{};
}

Texture::~Texture()
{
	if (m_buffer.data() != nullptr)
		deallocateInternalBuffer();
}

Texture& Texture::operator=(Texture&& other) noexcept
{
	if (this == &other)
		return *this;
	if (m_buffer.data() != nullptr)
		deallocateInternalBuffer();

	m_textureInfo = other.m_textureInfo;
	m_allocator = other.m_allocator;
	m_buffer = other.m_buffer;

	other.m_textureInfo = TextureInfo();
	other.m_allocator = nullptr;
	other.m_buffer = ByteSpan(nullptr, 0);

	return *this;
}

TextureInfo const& Texture::textureInfo() const
{
	return m_textureInfo;
}

FileFormat Texture::fileFormat() const
{
	return m_textureInfo.fileFormat;
}

TextureType Texture::textureType() const
{
	return m_textureInfo.textureType;
}

PixelFormat Texture::pixelFormat() const
{
	return m_textureInfo.pixelFormat;
}

ChannelType Texture::channelType() const
{
	return m_textureInfo.channelType;
}

ColorSpace Texture::colorSpace() const
{
	return m_textureInfo.colorSpace;
}

Dimensions Texture::baseDimensions() const
{
	return m_textureInfo.baseDimensions;
}

std::uint8_t Texture::mipCount() const
{
	return m_textureInfo.mipCount;
}

std::uint64_t Texture::layerCount() const
{
	return m_textureInfo.layerCount;
}

std::uint64_t Texture::mipOffset(std::uint8_t mipIndex) const
{
	return calculateMipOffset(m_textureInfo, mipIndex);
}

std::uint64_t Texture::mipSize(std::uint8_t mipIndex) const
{
	return layerSize(mipIndex) * m_textureInfo.layerCount;
}

std::byte const* Texture::mipData(std::uint8_t mipIndex) const
{
	return m_buffer.data() + mipOffset(mipIndex);;
}

ConstByteSpan Texture::mipSpan(std::uint8_t mipIndex) const
{
	return { mipData(mipIndex), static_cast<std::size_t>(mipSize(mipIndex)) };
}

std::uint64_t Texture::layerOffset(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return calculateLayerOffset(m_textureInfo, mipIndex, layerIndex);
}

std::uint64_t Texture::layerSize(std::uint8_t mipIndex) const
{
	return calculateSingleImageSize(
		calculateMipDimensions(m_textureInfo.baseDimensions, mipIndex),
		m_textureInfo.pixelFormat);
}

std::byte const* Texture::layerData(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return m_buffer.data() + layerOffset(mipIndex, layerIndex);
}

ConstByteSpan Texture::layerSpan(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return { layerData(mipIndex, layerIndex), static_cast<std::size_t>(layerSize(mipIndex)) };
}

std::byte const* Texture::rawBufferData() const
{
	return m_buffer.data();
}

std::uint64_t Texture::totalDataSize() const
{
	return calculateTotalSize(m_textureInfo);
}

ConstByteSpan Texture::rawBufferSpan() const
{
	return m_buffer;
}

void Texture::free()
{
	if (m_buffer.data() != nullptr)
		deallocateInternalBuffer();
}

void Texture::deallocateInternalBuffer()
{
	if (m_allocator != nullptr)
		m_allocator->deallocate(m_buffer.data(), Allocator::MemoryType::ImageData);
	else
	{
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
		delete m_buffer.data();
		m_buffer = {};
#else
		// We should never hit this path!
#endif
	}
}