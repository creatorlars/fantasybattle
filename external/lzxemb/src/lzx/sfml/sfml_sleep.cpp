#include <lzx/platform.hpp>
#include <thread>

namespace lzx
{
    void sleep(milliseconds duration)
    {
        std::this_thread::sleep_for(std::chrono::duration_cast<platform_clock::duration>(duration));
    }

    void sleep(microseconds duration)
    {
        std::this_thread::sleep_for(std::chrono::duration_cast<platform_clock::duration>(duration));
    }
    
    void sleep(seconds duration)
    {
        std::this_thread::sleep_for(std::chrono::duration_cast<platform_clock::duration>(duration));
    }

};