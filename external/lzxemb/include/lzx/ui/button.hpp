#pragma once

#include <lzx/hardware/pin.hpp>
#include <lzx/utility/math.hpp>
#include <lzx/utility/parameter.hpp>

namespace lzx
{

	constexpr uint32_t button_max_observers_v = 4;

    class button : public etl::observable<etl::observer<const bool&>, button_max_observers_v>
    {
    private:
    	button(const input_pin<bool>& driver, bool inverted) : m_driver(driver), m_inverted(inverted), m_state(m_inverted) {}

    	void poll()
    	{
    		bool state = m_inverted^m_driver.read();
    		if(m_state != state)
    		{
    			m_state = state;
    			notify_observers(m_state);
    		}
    	}

    private:
    	const input_pin<bool>& m_driver;
    	const bool m_inverted;
    	bool m_state;
    };
}
