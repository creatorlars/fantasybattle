#pragma once

#include "video_std.hpp"
#include <map>

namespace lzx
{

	static const inline double kVideoClockSD = 27000000.;
	static const inline double kVideoClockHD = 742500000.;
	static const inline double kVideoClockHD2 = kVideoClockHD / 1.001;

	struct video_format
	{
		uint32_t width;
		uint32_t height;
		bool interlaced;
		bool top_first;
		uint32_t get_frame_height() const
		{
			if (interlaced)
			{
				return height * 2;
			}
			return height;
		}
	};

	static const inline std::map<video_std, video_format> kVideoFormatMap = {
		{{video_std::PAL}, {720, 288, true, true}},
		{{video_std::NTSC}, {720, 243, true, false}},
		{{video_std::k480i5994}, {720, 240, true, false}},
		{{video_std::k576p50}, {720, 576, false, true}},
		{{video_std::k486p5994}, {720, 480, false, true}},
		{{video_std::k720p50}, {1280, 720, false, true}},
		{{video_std::k720p5994}, {1280, 720, false, true}},
		{{video_std::k720p60}, {1280, 720, false, true}},
		{{video_std::k1080i50}, {1920, 540, true, true}},
		{{video_std::k1080i5994}, {1920, 540, true, true}},
		{{video_std::k1080i60}, {1920, 540, true, true}},
		{{video_std::k1080p2398}, {1920, 1080, false, true}},
		{{video_std::k1080p24}, {1920, 1080, false, true}},
		{{video_std::k1080p25}, {1920, 1080, false, true}},
		{{video_std::k1080p2997}, {1920, 1080, false, true}},
		{{video_std::k1080p30}, {1920, 1080, false, true}},
		{{video_std::k640x480p60}, {640, 480, false, true}},
		{{video_std::k800x600p60}, {800, 600, false, true}},
		{{video_std::k1024x768p60}, {1024, 768, false, true}}};

}
