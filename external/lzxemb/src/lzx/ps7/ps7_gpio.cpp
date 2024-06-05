#include "ps7_gpio.hpp"

namespace lzx
{

	ps7_gpio::ps7_gpio(uint32_t dev_id) : m_initialized(false)
    {
            XGpioPs_Config *config = XGpioPs_LookupConfig(dev_id);
            if (config != nullptr)
            {
            	m_initialized = !(XGpioPs_CfgInitialize(&m_GpioPs, config, config->BaseAddr));
                if (m_initialized)
                {
                    // ("GPIO Controller %d Init OK, BaseAddr: 0x%08X", dev_id, config->BaseAddr);
                    if(!(XGpioPs_SelfTest(&m_GpioPs)))
                    {
                    	// ("GPIO Controller %d Self Test Success", dev_id);
                    }
                    else
                    {
                    	// ("GPIO Controller %d Self Test Failure", dev_id);
                    	m_initialized = false;
                    }
                }
                else
                {
                    // ("GPIO Controller %d Init Error", dev_id);
                }
            }
            else
            {
                // ("GPIO Controller %d Config Lookup Error", dev_id);
            }
    }

    bool ps7_gpio::is_initialized() const
    {
    	return m_initialized;
    }

    void ps7_gpio::write(uint32_t pin, const bool value)
    {
     	XGpioPs_SetOutputEnablePin(&m_GpioPs, (u32) pin, (u32) 1);
     	XGpioPs_SetDirectionPin(&m_GpioPs, (u32) pin, (u32) 1);
    	XGpioPs_WritePin(&m_GpioPs, (u32) pin, (u32) value);
//    	std::cout << "Wrote GPIO Pin " << pin << ", Value: " << value << std::endl;
    }

    bool ps7_gpio::read(uint32_t pin) const
    {
      	XGpioPs_SetOutputEnablePin(const_cast<XGpioPs*>(&m_GpioPs), (u32) pin, (u32) 0);
      	XGpioPs_SetDirectionPin(const_cast<XGpioPs*>(&m_GpioPs), (u32) pin, (u32) 0);
    	return !!(XGpioPs_ReadPin(const_cast<XGpioPs*>(&m_GpioPs), (u32) pin));
    }
}
