#pragma once

#include <lzx/hardware/pin.hpp>
#include <lzx/utility/math.hpp>
#include <lzx/utility/parameter.hpp>

namespace lzx
{


    class led : public etl::observer<const bool&>
    {
    private:
    	led(led_id id, output_pin<bool>& driver, bool inverted) : m_id(id), m_driver(driver), m_inverted(inverted), m_state(m_inverted) {}


    	void set(bool value)
    	{
    		bool state = m_inverted ^ value;
    		if(m_state != state)
    		{
    			m_state = state;
        		m_driver.write(m_state);
    		}
    	}

    	void notification(const bool& value)
    	{
			set(value);
    	}

    private:
    	const led_id m_id;
    	output_pin<bool>& m_driver;
    	const bool m_inverted;
    	led_state m_state;
    };
}
