#pragma once

#include <lzx/hardware/pin.hpp>

namespace lzx
{
    class ps7_coresight : public input_output_pin<uint8_t>
    {
    public:
        void write(const uint8_t);
        uint8_t read() const;
        bool is_data_available() const;
        bool is_space_available() const;
    };

}
