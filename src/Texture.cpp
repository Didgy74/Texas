#include "Texas/Texture.hpp"
#include "Texas/Tools.hpp"

#include "Texas/detail/Exception.hpp"

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

const Texas::TextureInfo& Texas::Texture::textureInfo() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .textureInfo() on empty Texture object."));
	return m_textureInfo;
}

Texas::FileFormat Texas::Texture::fileFormat() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .fileFormat() on empty Texture object."));
	return m_textureInfo.fileFormat;
}

Texas::TextureType Texas::Texture::textureType() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .textureType() on empty Texture object."));
	return m_textureInfo.textureType;
}

Texas::PixelFormat Texas::Texture::pixelFormat() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .pixelFormat() on empty Texture object."));
	return m_textureInfo.pixelFormat;
}

Texas::ChannelType Texas::Texture::channelType() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .channelType() on empty Texture object."));
	return m_textureInfo.channelType;
}

Texas::ColorSpace Texas::Texture::colorSpace() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .colorSpace() on empty Texture object."));
	return m_textureInfo.colorSpace;
}

Texas::Dimensions Texas::Texture::baseDimensions() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .baseDimensions() on empty Texture object."));
	return m_textureInfo.baseDimensions;
}

std::uint64_t Texas::Texture::mipLevelCount() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .mipLevelCount() on empty Texture object."));
	return m_textureInfo.mipLevelCount;
}

std::uint64_t Texas::Texture::arrayLayerCount() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .arrayLayerCount() on empty Texture object."));
	return m_textureInfo.arrayLayerCount;
}

std::uint64_t Texas::Texture::mipOffset(std::uint64_t mipLevelIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, std::out_of_range("Called .mipOffset() on empty Texture object."));
	return Texas::calcMipOffset(m_textureInfo, mipLevelIndex);
}

std::uint64_t Texas::Texture::mipSize(std::uint64_t mipLevelIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .mipSize() on empty Texture object.");
	return arrayLayerSize(mipLevelIndex) * m_textureInfo.arrayLayerCount;
}

const std::byte* Texas::Texture::mipData(std::uint64_t mipLevelIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .mipData() on empty Texture object.");
	return m_buffer.data() + mipOffset(mipLevelIndex);;
}

Texas::ConstByteSpan Texas::Texture::mipSpan(std::uint64_t mipLevelIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .mipSpan() on empty Texture object.");
	return ConstByteSpan(mipData(mipLevelIndex), static_cast<std::size_t>(mipSize(mipLevelIndex)));
}

std::uint64_t Texas::Texture::arrayLayerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .arrayLayerOffset() on empty Texture object.");
	return Texas::calcArrayLayerOffset(m_textureInfo, mipLevelIndex, arrayLayerIndex);
}

std::uint64_t Texas::Texture::arrayLayerSize(std::uint64_t mipIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .arrayLayerSize() on empty Texture object.");
	return Texas::calcSingleImageSize(Texas::calcMipDimensions(m_textureInfo.baseDimensions, mipIndex), m_textureInfo.pixelFormat);
}

const std::byte* Texas::Texture::arrayLayerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .arrayLayerData() on empty Texture object.");
	return m_buffer.data() + arrayLayerOffset(mipLevelIndex, arrayLayerIndex);
}

Texas::ConstByteSpan Texas::Texture::arrayLayerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .arrayLayerSpan() on empty Texture object.");
	return ConstByteSpan(arrayLayerData(mipLevelIndex, arrayLayerIndex), static_cast<std::size_t>(arrayLayerSize(mipLevelIndex)));
}

const std::byte* Texas::Texture::rawBufferData() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .rawBufferData() on empty Texture object.");
	return m_buffer.data();
}

std::uint64_t Texas::Texture::totalDataSize() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .totalDataSize() on empty Texture object.");
	return Texas::calcTotalSize(m_textureInfo);
}

Texas::ConstByteSpan Texas::Texture::rawBufferSpan() const
{
	TEXAS_DETAIL_EXCEPTION(m_buffer.data() != nullptr, "Called .rawBufferSpan() on empty Texture object.");
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
		TEXAS_DETAIL_EXCEPTION(
			true, 
			"Library bug. Hit a path where Texture's buffer needed to be deallocated, "
			"but no allocator was supplied and dynamic allocations are not enabled.");
#endif
	}
}