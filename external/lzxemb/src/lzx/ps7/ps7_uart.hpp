#pragma once

#include <lzx/hardware/uart.hpp>
#include <xuartps.h>

namespace lzx
{

    class ps7_uart : public uart
    {
    public:
        ps7_uart(const size_t handle);
        uint32_t write(const uint8_t*, const uint32_t) override;
        uint32_t read(uint8_t* const, const uint32_t) const override;
        bool is_output_available() const override;
        bool is_input_available() const override;
        void set_baud_rate(const size_t data_bits) override;
        void set_data_bits(const size_t data_bits) override;
        void set_stop_bits(const size_t stop_bits) override;
		void set_parity(const uart_parity parity) override;
		void set_flow_control(const uart_flow_control flow_control) override;
        size_t get_baud_rate() const override;
        size_t get_data_bits() const override;
        size_t get_stop_bits() const override;
		uart_parity get_parity() const override;
		uart_flow_control get_flow_control() const override;

    private:
        bool m_initialized;
        XUartPs m_UartPs;
    };

}
