#include <lzx/platform/clock.hpp>

namespace lzx
{
    platform_clock::time_point platform_clock::now()
    {
        return platform_clock::time_point(platform_clock::duration(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    }



};