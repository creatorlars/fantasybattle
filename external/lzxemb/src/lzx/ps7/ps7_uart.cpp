#include "ps7_uart.hpp"

namespace lzx
{


    void ps7_uart::set_data_bits(const size_t data_bits)
     {

     }
		
    void ps7_uart::set_stop_bits(const size_t stop_bits)
     {

     }

     void ps7_uart::set_parity(const uart_parity parity)
     {

     }

		void ps7_uart::set_flow_control(const uart_flow_control flow_control)
     {

     }

		size_t ps7_uart::get_baud_rate() const
     {
         return 0;
     }
		
     size_t ps7_uart::get_data_bits() const
     {
         return 0;
     }
		
     size_t ps7_uart::get_stop_bits() const
     {
         return 0;
     }

     uart_parity ps7_uart::get_parity() const
     {
         return uart_parity::none;
     }

	uart_flow_control ps7_uart::get_flow_control() const
     {
         return uart_flow_control::none;
     }

    ps7_uart::ps7_uart(const size_t dev_id) : m_initialized(false)
    {
            XUartPs_Config *config = XUartPs_LookupConfig(dev_id);
            if (config != nullptr)
            {
            	m_initialized = !(XUartPs_CfgInitialize(&m_UartPs, config, config->BaseAddress));
                if (m_initialized)
                {
//                    // ("UART Controller %d Init OK, BaseAddr: 0x%08X, InputClockHz: %d, ModemPinsConnected: %d, BaudRate: %d", dev_id, config->BaseAddress, config->InputClockHz, config->ModemPinsConnected, m_UartPs.BaudRate);

                    if (!(XUartPs_SelfTest(&m_UartPs)))
                    {
//                        // ("UART Controller %d Self Test Success", dev_id);
                    }
                    else
                    {
//                        // ("UART Controller %d Self Test Failure", dev_id);
                        m_initialized = false;
                    }
                }
                else
                {
//                    // ("UART Controller %d Init Error", dev_id);
                }
            }
            else
            {
//                // ("UART Controller %d Config Lookup Error", dev_id);
            }

    }

    uint32_t ps7_uart::write(const uint8_t* data, const uint32_t size)
    {
    	size_t bw = 0;
    	for(size_t it = 0; it < size; it++)
    	{
    		XUartPs_SendByte(m_UartPs.Config.BaseAddress, data[it]);
    		bw++;
    	}
    	return bw;
    }

    uint32_t ps7_uart::read(uint8_t* const data, const uint32_t size) const
    {
    	size_t br = 0;
    	while(is_input_available() && br < size)
    	{
    		data[br] = XUartPs_RecvByte(m_UartPs.Config.BaseAddress);
    		br++;
    	}
    	return br;
    }

    bool ps7_uart::is_input_available() const
    {
    	return !!(XUartPs_IsReceiveData(m_UartPs.Config.BaseAddress));
    }

    bool ps7_uart::is_output_available() const
    {
        return !(XUartPs_IsTransmitFull(m_UartPs.Config.BaseAddress));
    }

    void ps7_uart::set_baud_rate(const size_t baud_rate)
    {
        XUartPs_SetBaudRate(&m_UartPs, baud_rate);
    }




}
