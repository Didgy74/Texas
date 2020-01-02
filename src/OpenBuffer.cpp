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
