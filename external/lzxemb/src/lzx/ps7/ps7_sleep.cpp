#include <lzx/platform/sleep.hpp>
#include <xenv.h>

namespace lzx
{

    void sleep(milliseconds duration)
    {
        XENV_USLEEP(duration.count() * 1000);
    }

    void sleep(microseconds duration)
    {
        XENV_USLEEP(duration.count());
    }

};
