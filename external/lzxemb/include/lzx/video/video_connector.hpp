#pragma once

#include <stdint.h>

namespace lzx
{
    enum class video_connector : uint32_t
    {
        kComposite = 0,
        kSVideo,
        kComponent,
        kGraphics,
        kScartCVBS
    };
}
