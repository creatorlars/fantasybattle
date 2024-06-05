#pragma once

#include <cmath>

namespace lzx
{
    enum class curve
    {
        kLinear = 0,
        kEaseInSine,
        kEaseOutSine,
        kEaseInOutSine,
        kEaseInQuad,
        kEaseOutQuad,
        kEaseInOutQuad,
        kEaseInCubic,
        kEaseOutCubic,
        kEaseInOutCubic,
        kEaseInQuart,
        kEaseOutQuart,
        kEaseInOutQuart,
        kEaseInQuint,
        kEaseOutQuint,
        kEaseInOutQuint,
        kEaseInExpo,
        kEaseOutExpo,
        kEaseInOutExpo,
        kEaseInCirc,
        kEaseOutCirc,
        kEaseInOutCirc,
        kEaseInBack,
        kEaseOutBack,
        kEaseInOutBack,
        kEaseInElastic,
        kEaseOutElastic,
        kEaseInOutElastic,
        kEaseInBounce,
        kEaseOutBounce,
        kEaseInOutBounce,
        kTriangle,
        kSquare,
        kSaw,
        kSine,
        kCosine,
        kNumItems
    };

    const inline char *kCurveStrings[] = {
        "Linear",
        "EaseInSine",
        "EaseOutSine",
        "EaseInOutSine",
        "EaseInQuad",
        "EaseOutQuad",
        "EaseInOutQuad",
        "EaseInCubic",
        "EaseOutCubic",
        "EaseInOutCubic",
        "EaseInQuart",
        "EaseOutQuart",
        "EaseInOutQuart",
        "EaseInQuint",
        "EaseOutQuint",
        "EaseInOutQuint",
        "EaseInExpo",
        "EaseOutExpo",
        "EaseInOutExpo",
        "EaseInCirc",
        "EaseOutCirc",
        "EaseInOutCirc",
        "EaseInBack",
        "EaseOutBack",
        "EaseInOutBack",
        "EaseInElastic",
        "EaseOutElastic",
        "EaseInOutElastic",
        "EaseInBounce",
        "EaseOutBounce",
        "EaseInOutBounce",
        "Triangle",
        "Square",
        "Saw",
        "Sine",
        "Cosine"};

   const inline char *kCurveStringSepZero = "Linear\0EaseInSine\0EaseOutSine\0EaseInOutSine\0EaseInQuad\0EaseOutQuad\0EaseInOutQuad\0EaseInCubic\0EaseOutCubic\0EaseInOutCubic\0EaseInQuart\0EaseOutQuart\0EaseInOutQuart\0EaseInQuint\0EaseOutQuint\0EaseInOutQuint\0EaseInExpo\0EaseOutExpo\0EaseInOutExpo\0EaseInCirc\0EaseOutCirc\0EaseInOutCirc\0EaseInBack\0EaseOutBack\0EaseInOutBack\0EaseInElastic\0EaseOutElastic\0EaseInOutElastic\0EaseInBounce\0EaseOutBounce\0EaseInOutBounce\0Triangle\0Square\0Saw\0Sine\0Cosine";

}
