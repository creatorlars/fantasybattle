#include <lzx/platform/clock.hpp>
#include <xil_io.h>
#include <xtime_l.h>

namespace lzx
{

    platform_clock::time_point platform_clock::now()
    {
		volatile u32 low;
		volatile u32 high;

		/* Reading Global Timer Counter Register */
		do
		{
			high = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET);
			low = Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_LOWER_OFFSET);
		} while(Xil_In32(GLOBAL_TMR_BASEADDR + GTIMER_COUNTER_UPPER_OFFSET) != high);

		volatile XTime global_timer_count = (((XTime) high) << 32U) | (XTime) low;

        return platform_clock::time_point(platform_clock::duration(static_cast<uint64_t>(global_timer_count)));
    }

};
