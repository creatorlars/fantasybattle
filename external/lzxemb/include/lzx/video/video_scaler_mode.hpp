#pragma once

#include <stdint.h>

namespace lzx
{
    enum class video_scaler_mode : uint32_t
	{
        Stretch = 0,
        Crop,
        Fit
	};
}
