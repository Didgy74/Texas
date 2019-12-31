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
		if (m_metaData.srcFileFormat == FileFormat::KTX)
		{
			return 0;
		}
		else
			return 0;
	}
}
