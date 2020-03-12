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

std::uint64_t Texas::Texture::mipLevelCount() const
{
	return m_textureInfo.mipLevelCount;
}

std::uint64_t Texas::Texture::arrayLayerCount() const
{
	return m_textureInfo.arrayLayerCount;
}

std::uint64_t Texas::Texture::mipOffset(std::uint64_t mipLevelIndex) const
{
	return Texas::calcMipOffset(m_textureInfo, mipLevelIndex);
}

std::uint64_t Texas::Texture::mipSize(std::uint64_t mipLevelIndex) const
{
	return layerSize(mipLevelIndex) * m_textureInfo.arrayLayerCount;
}

std::byte const* Texas::Texture::mipData(std::uint64_t mipLevelIndex) const
{
	return m_buffer.data() + mipOffset(mipLevelIndex);;
}

Texas::ConstByteSpan Texas::Texture::mipSpan(std::uint64_t mipLevelIndex) const
{
	return { mipData(mipLevelIndex), static_cast<std::size_t>(mipSize(mipLevelIndex)) };
}

std::uint64_t Texas::Texture::layerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	return Texas::calcLayerOffset(m_textureInfo, mipLevelIndex, arrayLayerIndex);
}

std::uint64_t Texas::Texture::layerSize(std::uint64_t mipIndex) const
{
	return Texas::calcSingleImageSize(
		Texas::calcMipDimensions(
			m_textureInfo.baseDimensions, 
			mipIndex), 
		m_textureInfo.pixelFormat);
}

std::byte const* Texas::Texture::layerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	return m_buffer.data() + layerOffset(mipLevelIndex, arrayLayerIndex);
}

Texas::ConstByteSpan Texas::Texture::layerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	return { layerData(mipLevelIndex, arrayLayerIndex), 
			 static_cast<std::size_t>(layerSize(mipLevelIndex)) };
}

std::byte const* Texas::Texture::rawBufferData() const
{
	return m_buffer.data();
}

std::uint64_t Texas::Texture::totalDataSize() const
{
	return Texas::calcTotalSize(m_textureInfo);
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