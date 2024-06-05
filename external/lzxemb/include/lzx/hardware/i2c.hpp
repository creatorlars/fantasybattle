#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
    class i2c
    {
    public:
        virtual ~i2c() {}
        virtual void write_reg(uint8_t device_address, uint8_t reg_address, uint8_t reg_value) = 0;
        virtual uint8_t read_reg(uint8_t device_address, uint8_t reg_address) const = 0;
        virtual void write_bytes(uint8_t addr, uint8_t* data, size_t data_size) = 0;
        virtual void read_bytes(uint8_t addr, uint8_t*const data, size_t data_size) = 0;
        virtual bool probe_addr(uint8_t addr) = 0;
    };
}
