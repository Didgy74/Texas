#include "Texas/FileInfo.hpp"

const Texas::TextureInfo& Texas::FileInfo::textureInfo() const noexcept
{
    return m_textureInfo;
}

Texas::Dimensions Texas::FileInfo::baseDimensions() const noexcept
{
    return m_textureInfo.baseDimensions;
}

std::uint64_t Texas::FileInfo::memoryRequired() const noexcept
{
    return m_memoryRequired;
}

std::uint64_t Texas::FileInfo::workingMemoryRequired() const noexcept
{
    return m_workingMemoryRequired;
}
