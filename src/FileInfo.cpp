#include <Texas/FileInfo.hpp>

using namespace Texas;

TextureInfo const& FileInfo::textureInfo() const noexcept
{
	return m_textureInfo;
}

std::uint64_t FileInfo::memoryRequired() const noexcept
{
	return m_memoryRequired;
}

std::uint64_t FileInfo::workingMemoryRequired() const noexcept
{
	return m_workingMemoryRequired;
}
