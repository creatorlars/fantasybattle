#pragma once

#include <lzxemb_config.hpp>
#include <chrono>

namespace lzx
{
    struct platform_clock
    {
        typedef int64_t rep;
        typedef std::ratio<1, LZXEMB_PLATFORM_CLOCK_COUNTS_PER_SECOND> period;
        typedef std::chrono::duration<rep, period> duration;
        typedef std::chrono::time_point<platform_clock> time_point;
        static constexpr bool is_steady = true;
        static constexpr double frequency = double(period::num) / double(period::den);
        static time_point now();
    };

    template <typename PeriodType>
    struct clock
    {
        typedef int64_t rep;
        typedef PeriodType period;
        typedef clock<period> clock_type;
        typedef std::chrono::duration<rep, period> duration;
        typedef std::chrono::time_point<clock_type> time_point;
        static constexpr bool is_steady = true;
        static constexpr double frequency = double(period::num) / double(period::den);
        static inline time_point now()
        {
            return time_point(std::chrono::duration_cast<duration>(platform_clock::now().time_since_epoch()));
        }
    };

};
