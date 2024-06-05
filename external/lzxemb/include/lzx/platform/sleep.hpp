#pragma once

#include <lzx/platform/time.hpp>

namespace lzx
{
    void sleep(milliseconds duration);
    void sleep(microseconds duration);
    void sleep(seconds duration);
};
