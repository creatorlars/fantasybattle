#pragma once

#include <chrono>

namespace lzx
{
    typedef std::chrono::duration<int64_t, std::ratio<1, 1000000>> microseconds;
    typedef std::chrono::duration<int64_t, std::ratio<1, 1000>> milliseconds;
    typedef std::chrono::duration<double, std::ratio<1>> seconds;
};
