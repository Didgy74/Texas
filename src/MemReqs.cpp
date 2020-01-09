#include "Texas/OpenBuffer.hpp"

#include "Texas/Tools.hpp"

namespace Texas
{
    std::size_t OpenBuffer::mipOffset(const std::uint32_t mipIndex) const noexcept
    {
        if (mipIndex == 0)
            return 0;
        else
            return Tools::CalcTotalSizeRequired(m_metaData.baseDimensions, mipIndex, m_metaData.arrayLayerCount, m_metaData.pixelFormat);
    }
}
