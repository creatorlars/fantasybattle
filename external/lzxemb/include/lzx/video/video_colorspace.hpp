#pragma once

#include <stdint.h>

namespace lzx
{
    enum class video_colorspace : uint32_t
    {
        kYPbPr = 0,
        kRGB
    };
}
