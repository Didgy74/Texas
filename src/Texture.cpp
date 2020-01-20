#include "Texas/Texture.hpp"
#include "Texas/Tools.hpp"

#include "Texas/detail/Assert.hpp"

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

const Texas::TextureInfo& Texas::Texture::textureInfo() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .textureInfo() on empty Texture object.");
	return m_textureInfo;
}

Texas::FileFormat Texas::Texture::fileFormat() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .srcFileFormat() on empty Texture object.");
	return m_textureInfo.fileFormat;
}

Texas::TextureType Texas::Texture::textureType() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .textureType() on empty Texture object.");
	return m_textureInfo.textureType;
}

Texas::PixelFormat Texas::Texture::pixelFormat() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .pixelFormat() on empty Texture object.");
	return m_textureInfo.pixelFormat;
}

Texas::ChannelType Texas::Texture::channelType() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .channelType() on empty Texture object.");
	return m_textureInfo.channelType;
}

Texas::ColorSpace Texas::Texture::colorSpace() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .colorSpace() on empty Texture object.");
	return m_textureInfo.colorSpace;
}

Texas::Dimensions Texas::Texture::baseDimensions() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .baseDimensions() on empty Texture object.");
	return m_textureInfo.baseDimensions;
}

std::uint64_t Texas::Texture::mipLevelCount() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .mipLevelCount() on empty Texture object.");
	return m_textureInfo.mipLevelCount;
}

std::uint64_t Texas::Texture::arrayLayerCount() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .arrayLayerCount() on empty Texture object.");
	return m_textureInfo.arrayLayerCount;
}

Texas::Optional<std::uint64_t> Texas::Texture::mipOffset(std::uint64_t mipLevelIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .mipOffset() on empty Texture object.");
	return Texas::calcMipOffset(m_textureInfo, mipLevelIndex);
}

Texas::Optional<std::uint64_t> Texas::Texture::mipSize(std::uint64_t mipLevelIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .mipSize() on empty Texture object.");

	auto singleArrayLayerSize = arrayLayerSize(mipLevelIndex);
	if (!singleArrayLayerSize.hasValue())
		return {};
	return { singleArrayLayerSize.value() * m_textureInfo.arrayLayerCount };
}

Texas::Optional<const std::byte*> Texas::Texture::mipData(std::uint64_t mipLevelIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .mipData() on empty Texture object.");

	auto mipOffsetOpt = mipOffset(mipLevelIndex);
	if (mipOffsetOpt.hasValue())
		return { m_buffer.data() + mipOffsetOpt.value() };
	return {};
}

Texas::Optional<Texas::ConstByteSpan> Texas::Texture::mipSpan(std::uint64_t mipLevelIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .mipSpan() on empty Texture object.");

	auto mipDataOpt = mipData(mipLevelIndex);
	if (!mipDataOpt.hasValue())
		return {};
	auto mipSizeOpt = mipSize(mipLevelIndex);
	if (!mipSizeOpt.hasValue())
		return {};

	return { ConstByteSpan(mipDataOpt.value(), mipSizeOpt.value()) };
}

Texas::Optional<std::uint64_t> Texas::Texture::arrayLayerOffset(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .arrayLayerOffset() on empty Texture object.");

	return Texas::calcArrayLayerOffset(m_textureInfo, mipLevelIndex, arrayLayerIndex);
}

Texas::Optional<std::uint64_t> Texas::Texture::arrayLayerSize(std::uint64_t mipIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .arrayLayerSize() on empty Texture object.");

	auto dimsOpt = Texas::calcMipmapDimensions(m_textureInfo.baseDimensions, mipIndex);
	if (!dimsOpt.hasValue())
		return {};
	return Texas::calcArrayLayerSize(dimsOpt.value(), m_textureInfo.pixelFormat);
}

Texas::Optional<const std::byte*> Texas::Texture::arrayLayerData(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .arrayLayerData() on empty Texture object.");

	auto arrayOffsetOpt = arrayLayerOffset(mipLevelIndex, arrayLayerIndex);
	if (arrayOffsetOpt.hasValue())
		return { m_buffer.data() + arrayOffsetOpt.value() };
	return {};
}

Texas::Optional<Texas::ConstByteSpan> Texas::Texture::arrayLayerSpan(std::uint64_t mipLevelIndex, std::uint64_t arrayLayerIndex) const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .arrayLayerSpan() on empty Texture object.");

	auto arrayLayerDataOpt = arrayLayerData(mipLevelIndex, arrayLayerIndex);
	if (!arrayLayerDataOpt.hasValue())
		return {};
	auto arrayLayerSizeOpt = arrayLayerSize(mipLevelIndex);
	if (!arrayLayerSizeOpt.hasValue())
		return {};

	return { ConstByteSpan(arrayLayerDataOpt.value(), arrayLayerSizeOpt.value()) };
}

const std::byte* Texas::Texture::rawBufferData() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .rawBufferData() on empty Texture object.");
	return m_buffer.data();
}

std::uint64_t Texas::Texture::totalDataSize() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .totalDataSize() on empty Texture object.");
	return Texas::calcTotalSizeRequired(m_textureInfo);
}

Texas::ConstByteSpan Texas::Texture::rawBufferSpan() const noexcept
{
	TEXAS_DETAIL_ASSERT_MSG(m_buffer.data() != nullptr, "Called .rawBufferSpan() on empty Texture object.");
	return m_buffer;
}

void Texas::Texture::deallocateInternalBuffer()
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