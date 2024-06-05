#pragma once

#include "pin.hpp"
#include "gpio.hpp"

namespace lzx
{

    class gpio_pin : public input_output_pin<bool>
    {
    public:
        gpio_pin(gpio &driver, const size_t pin) : m_driver(driver), m_pin(pin) {}
        virtual ~gpio_pin() {}

        bool read() const override
        {
        	return m_driver.read(m_pin);
        }

        void write(const bool state) override
        {
			m_driver.write(m_pin, state);
        }

    private:
        gpio &m_driver;
        const size_t m_pin;
    };
}
