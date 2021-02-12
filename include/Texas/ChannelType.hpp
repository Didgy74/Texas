#pragma once

namespace Texas
{
	/*
		Provides a hint as to how each channel of a texture should be interpretted.
	*/
	enum class ChannelType : char
	{
		Invalid,

		UnsignedNormalized,
		SignedNormalized,

		UnsignedScaled,
		SignedScaled,

		UnsignedInteger,
		SignedInteger,

		UnsignedFloat,
		SignedFloat,

		sRGB,
	};
}
