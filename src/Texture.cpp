#include "Texas/Texture.hpp"
#include "Texas/Tools.hpp"

Texas::Texture::Texture(Texture&& in) noexcept
{
	m_metaData = in.m_metaData;
	m_allocator = in.m_allocator;
	m_buffer = in.m_buffer;

	in.m_metaData = MetaData{};
	in.m_allocator = nullptr;
	in.m_buffer = ByteSpan{};
}

Texas::Texture::~Texture()
{
	// Deallocate current buffer
	if (m_buffer.data() != nullptr)
	{
		if (m_allocator != nullptr)
			m_allocator->deallocate(m_buffer.data());
		else
		{
#ifdef TEXAS_ENABLE_DYNAMIC_ALLOCATIONS
			delete m_buffer.data();
#else
			TEXAS_DETAIL_ASSERT_MSG(false, 
				"Author error. Texture should deallocate internal buffer, " 
				"but allocator is nullptr and dynamic allocations are not enabled.");
#endif
		}
	}
}

Texas::Texture& Texas::Texture::operator=(Texture&& other) noexcept
{
	if (this == &other)
		return *this;

	this->~Texture();

	m_metaData = other.m_metaData;
	m_allocator = other.m_allocator;
	m_buffer = other.m_buffer;

	other.m_metaData = MetaData();
	other.m_allocator = nullptr;
	other.m_buffer = ByteSpan(nullptr, 0);

	return *this;
}

const Texas::MetaData& Texas::Texture::metaData() const noexcept
{
	return m_metaData;
}

Texas::FileFormat Texas::Texture::srcFileFormat() const noexcept
{
	return m_metaData.srcFileFormat;
}

Texas::TextureType Texas::Texture::textureType() const noexcept
{
	return m_metaData.textureType;
}

Texas::PixelFormat Texas::Texture::pixelFormat() const noexcept
{
	return m_metaData.pixelFormat;
}

Texas::ChannelType Texas::Texture::channelType() const noexcept
{
	return m_metaData.channelType;
}

Texas::ColorSpace Texas::Texture::colorSpace() const noexcept
{
	return m_metaData.colorSpace;
}

Texas::Dimensions Texas::Texture::baseDimensions() const noexcept
{
	return m_metaData.baseDimensions;
}

std::uint64_t Texas::Texture::mipLevelCount() const noexcept
{
	return m_metaData.mipLevelCount;
}

std::uint64_t Texas::Texture::arrayLayerCount() const noexcept
{
	return m_metaData.arrayLayerCount;
}

Texas::Optional<std::uint64_t> Texas::Texture::mipOffset(std::uint64_t mipLevelIndex) const noexcept
{
	return Texas::calcMipOffset(m_metaData, mipLevelIndex);
}

Texas::Optional<std::uint64_t> Texas::Texture::mipSize(std::uint64_t mipLevelIndex) const noexcept
{
	auto singleArrayLayerSize = arrayLayerSize(mipLevelIndex);
	if (!singleArrayLayerSize.hasValue())
		return {};
	return { singleArrayLayerSize.value() * m_metaData.arrayLayerCount };
}

const std::byte* Texas::Texture::mipData(std::uint64_t mipLevelIndex) const noexcept
{
	if (m_buffer.data() != nullptr)
	{
		auto mipOffsetOpt = mipOffset(mipLevelIndex);
		if (mipOffsetOpt.hasValue())
			return m_buffer.data() + mipOffsetOpt.value();
	}
	
	return nullptr;
}

Texas::Optional<std::uint64_t> Texas::Texture::arrayLayerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept
{
	return Texas::calcArrayLayerOffset(m_metaData, mipLevelIndex, arrayLayerIndex);
}

Texas::Optional<std::uint64_t> Texas::Texture::arrayLayerSize(std::uint64_t mipIndex) const noexcept
{
	auto dims = Texas::calcMipmapDimensions(m_metaData.baseDimensions, mipIndex);
	if (dims.width == 0 && dims.height == 0 && dims.depth == 0)
		return {};
	return Texas::calcSingleImageDataSize(dims, m_metaData.pixelFormat);
}

const std::byte* Texas::Texture::arrayLayerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept
{
	if (m_buffer.data() != nullptr)
	{
		auto arrayOffsetOpt = arrayLayerOffset(mipLevelIndex, arrayLayerIndex);
		if (arrayOffsetOpt.hasValue())
			return m_buffer.data() + arrayOffsetOpt.value();
	}
	return nullptr;
}

const std::byte* Texas::Texture::rawBufferData() const noexcept
{
	return m_buffer.data();
}

std::uint64_t Texas::Texture::totalDataSize() const noexcept
{
	return Texas::calcTotalSizeRequired(m_metaData);
}

Texas::ConstByteSpan Texas::Texture::rawBufferSpan() const noexcept
{
	return m_buffer;
}

bool Texas::Texture::isValid() const noexcept
{
	return m_buffer.data() != nullptr;
}