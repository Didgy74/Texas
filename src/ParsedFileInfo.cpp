#include "Texas/ParsedFileInfo.hpp"

const Texas::TextureInfo& Texas::ParsedFileInfo::textureInfo() const noexcept
{
    return m_textureInfo;
}

Texas::Dimensions Texas::ParsedFileInfo::baseDimensions() const noexcept
{
    return m_textureInfo.baseDimensions;
}

std::uint64_t Texas::ParsedFileInfo::memoryRequired() const noexcept
{
    return m_memoryRequired;
}

std::uint64_t Texas::ParsedFileInfo::workingMemoryRequired() const noexcept
{
    return m_workingMemoryRequired;
}
