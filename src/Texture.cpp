#include "Texas/Texture.hpp"
#include "Texas/Tools.hpp"

Texas::Texture::Texture(Texture&& in) noexcept
{
	m_textureInfo = in.m_textureInfo;
	m_allocator = in.m_allocator;
	m_buffer = in.m_buffer;

	in.m_textureInfo = TextureInfo{};
	in.m_allocator = nullptr;
	in.m_buffer = ByteSpan{};
}

Texas::Texture::~Texture()
{
	if (m_buffer.data() != nullptr)
		deallocateInternalBuffer();
}

Texas::Texture& Texas::Texture::operator=(Texture&& other) noexcept
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

Texas::TextureInfo const& Texas::Texture::textureInfo() const
{
	return m_textureInfo;
}

Texas::FileFormat Texas::Texture::fileFormat() const
{
	return m_textureInfo.fileFormat;
}

Texas::TextureType Texas::Texture::textureType() const
{
	return m_textureInfo.textureType;
}

Texas::PixelFormat Texas::Texture::pixelFormat() const
{
	return m_textureInfo.pixelFormat;
}

Texas::ChannelType Texas::Texture::channelType() const
{
	return m_textureInfo.channelType;
}

Texas::ColorSpace Texas::Texture::colorSpace() const
{
	return m_textureInfo.colorSpace;
}

Texas::Dimensions Texas::Texture::baseDimensions() const
{
	return m_textureInfo.baseDimensions;
}

std::uint8_t Texas::Texture::mipCount() const
{
	return m_textureInfo.mipCount;
}

std::uint64_t Texas::Texture::layerCount() const
{
	return m_textureInfo.layerCount;
}

std::uint64_t Texas::Texture::mipOffset(std::uint8_t mipIndex) const
{
	return Texas::calculateMipOffset(m_textureInfo, mipIndex);
}

std::uint64_t Texas::Texture::mipSize(std::uint8_t mipIndex) const
{
	return layerSize(mipIndex) * m_textureInfo.layerCount;
}

std::byte const* Texas::Texture::mipData(std::uint8_t mipIndex) const
{
	return m_buffer.data() + mipOffset(mipIndex);;
}

Texas::ConstByteSpan Texas::Texture::mipSpan(std::uint8_t mipIndex) const
{
	return { mipData(mipIndex), mipSize(mipIndex) };
}

std::uint64_t Texas::Texture::layerOffset(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return Texas::calculateLayerOffset(m_textureInfo, mipIndex, layerIndex);
}

std::uint64_t Texas::Texture::layerSize(std::uint8_t mipIndex) const
{
	return Texas::calculateSingleImageSize(
		Texas::calculateMipDimensions(
			m_textureInfo.baseDimensions, 
			mipIndex), 
		m_textureInfo.pixelFormat);
}

std::byte const* Texas::Texture::layerData(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return m_buffer.data() + layerOffset(mipIndex, layerIndex);
}

Texas::ConstByteSpan Texas::Texture::layerSpan(std::uint8_t mipIndex, std::uint64_t layerIndex) const
{
	return { layerData(mipIndex, layerIndex), 
			 static_cast<std::size_t>(layerSize(mipIndex)) };
}

std::byte const* Texas::Texture::rawBufferData() const
{
	return m_buffer.data();
}

std::uint64_t Texas::Texture::totalDataSize() const
{
	return Texas::calculateTotalSize(m_textureInfo);
}

Texas::ConstByteSpan Texas::Texture::rawBufferSpan() const
{
	return m_buffer;
}

void Texas::Texture::deallocateInternalBuffer()
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