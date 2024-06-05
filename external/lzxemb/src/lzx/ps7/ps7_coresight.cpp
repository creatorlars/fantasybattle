#include "ps7_coresight.hpp"
#include <xil_types.h>
#include <xpseudo_asm.h>

namespace lzx
{

#define XCORESIGHTPS_DCC_STATUS_RX (1 << 30)
#define XCORESIGHTPS_DCC_STATUS_TX (1 << 29)

    static inline uint32_t XCoresightPs_DccGetStatus(void)
    {
        uint32_t Status = 0U;
        asm volatile("mrc p14, 0, %0, c0, c1, 0"
                     : "=r"(Status)
                     :
                     : "cc");
        return Status;
    }

    static inline void XCoresightPs_DccSendByte(const uint8_t Data)
    {
        while (XCoresightPs_DccGetStatus() & XCORESIGHTPS_DCC_STATUS_TX)
            dsb();
        asm volatile("mcr p14, 0, %0, c0, c5, 0"
                     :
                     : "r"(Data));
        isb();
    }

    static inline uint8_t XCoresightPs_DccRecvByte()
    {
        uint8_t Data = 0U;

        while (!(XCoresightPs_DccGetStatus() & XCORESIGHTPS_DCC_STATUS_RX))
        {
            dsb();
        }

        asm volatile("mrc p14, 0, %0, c0, c5, 0"
                     : "=r"(Data));
        isb();

        return Data;
    }

    inline void ps7_coresight::write(const uint8_t c)
    {
        XCoresightPs_DccSendByte(c);
    }

    inline uint8_t ps7_coresight::read() const
    {
        return XCoresightPs_DccRecvByte();
    }

    inline bool ps7_coresight::is_data_available() const
    {
        return !!(XCoresightPs_DccGetStatus() & XCORESIGHTPS_DCC_STATUS_RX);
    }

    inline bool ps7_coresight::is_space_available() const
    {
        return true;
    }

#undef XCORESIGHTPS_DCC_STATUS_RX
#undef XCORESIGHTPS_DCC_STATUS_TX

}
