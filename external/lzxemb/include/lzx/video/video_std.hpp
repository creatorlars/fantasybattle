#pragma once

#include <stdint.h>

namespace lzx
{
	enum class video_std : uint32_t
	{
				PAL = 0,
				NTSC,
				k480i5994,
				k576p50,
				k486p5994,
				k720p50,
				k720p5994,
				k720p60,
				k1080i50,
				k1080i5994,
				k1080i60,
				k1080p2398,
				k1080p24,
				k1080psf2398,
				k1080psf24,
				k1080p25,
				k1080p2997,
				k1080p30,
				k640x480p60,
				k800x600p60,
				k1024x768p60
	};

}
