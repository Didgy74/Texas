#include "Texas/OpenBuffer.hpp"

#include "Texas/Tools.hpp"

namespace Texas
{
    std::size_t OpenBuffer::memoryRequired() const
    {
        return Tools::CalcTotalSizeRequired(m_metaData.baseDimensions, m_metaData.mipLevelCount, m_metaData.arrayLayerCount, m_metaData.pixelFormat);
    }

    std::size_t OpenBuffer::workingMemoryRequired() const
    {
#ifdef TEXAS_ENABLE_KTX_READ
        if (m_metaData.srcFileFormat == FileFormat::KTX)
        {
            return 0;
        }
#endif

#ifdef TEXAS_ENABLE_PNG_READ
        if (m_metaData.srcFileFormat == FileFormat::PNG)
        {
            std::size_t totalSize = 0;

            // We start by getting the amount of pixels in the image.
            totalSize += static_cast<std::size_t>(baseDimensions().width) * static_cast<std::size_t>(baseDimensions().height);

            // Then we grab bytes required by finding how many bytes one pixel is.
            std::uint8_t pixelByteLength = 0;
            switch (m_metaData.pixelFormat)
            {
            case PixelFormat::R_8:
                pixelByteLength = sizeof(std::uint8_t);
                break;
            case PixelFormat::R_16:
                pixelByteLength = sizeof(std::uint8_t) * 2;
                break;
            case PixelFormat::RA_8:
                pixelByteLength = sizeof(std::uint8_t) * 2;
            case PixelFormat::RA_16:
                pixelByteLength = sizeof(std::uint8_t) * 4;
            case PixelFormat::RGB_8:
                pixelByteLength = sizeof(std::uint8_t) * 3;
                break;
            case PixelFormat::RGB_16:
                pixelByteLength = sizeof(std::uint8_t) * 6;
                break;
            case PixelFormat::RGBA_8:
                pixelByteLength = sizeof(std::uint8_t) * 4;
                break;
            case PixelFormat::RGBA_16:
                pixelByteLength = sizeof(std::uint8_t) * 8;
                break;
            }

            totalSize *= pixelByteLength;

            // We add one byte for every row, because every row starts with a defiltering uint8 type
            totalSize += baseDimensions().height;

            return totalSize;
        }
#endif
        
        return 0;
    }

    std::size_t OpenBuffer::mipOffset(const std::uint32_t mipIndex) const
    {
        if (mipIndex == 0)
            return 0;
        else
            return Tools::CalcTotalSizeRequired(m_metaData.baseDimensions, mipIndex, m_metaData.arrayLayerCount, m_metaData.pixelFormat);
    }
}
