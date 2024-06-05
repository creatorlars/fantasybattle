#pragma once

#include <lzx/hardware/gpio.hpp>
#include <xgpiops.h>

namespace lzx
{
    class ps7_gpio : public gpio
    {
    public:
        ps7_gpio(uint32_t devId);

        void write(uint32_t pin_number, const bool value) override;
        bool read(uint32_t pin_number) const override;
        bool is_initialized() const;

    private:
        XGpioPs m_GpioPs;
        bool m_initialized;
    };

}
