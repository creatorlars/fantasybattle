#pragma once

#include <lzx/platform.hpp>

namespace lzx
{

    class gpio
    {
    public:
        virtual ~gpio() {}
        virtual void write(uint32_t pin_number, bool value) = 0;
        virtual bool read(uint32_t pin_number) const = 0;
    };

}
