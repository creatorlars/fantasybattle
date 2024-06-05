#pragma once

#include <lzx/hardware/i2c.hpp>
#include <xiicps.h>

namespace lzx
{
    class ps7_i2c : public i2c
    {
    public:
        ps7_i2c(uint16_t handle, uint32_t clock_frq);
        void write_reg(uint8_t device_address, uint8_t reg_address, uint8_t reg_value) override;
        uint8_t read_reg(uint8_t device_address, uint8_t reg_address) const override;
        bool probe_addr(uint8_t addr) override;
        void write_bytes(uint8_t i2cAddr, uint8_t *buf, size_t numBytes) override;
        void read_bytes(uint8_t i2cAddr, uint8_t *const buf, size_t numBytes) override;

    private:
        bool m_init;
        XIicPs m_IicPs;
        int32_t m_status;
    };

}
