#pragma once

#include <lzx/hardware/stream.hpp>

namespace lzx
{

	enum class uart_parity : size_t
	{
		none,
		odd,
		even,
		mark,
		space
	};

	enum class uart_flow_control : size_t
	{
		none,
		xon_xoff,
		rts_cts,
		dsr_dtr
	};

	class uart : public input_output_stream<uint8_t>
	{
	public:
		virtual ~uart() {}
		virtual void set_baud_rate(const size_t baud_rate) = 0;
		virtual void set_data_bits(const size_t data_bits) = 0;
		virtual void set_stop_bits(const size_t stop_bits) = 0;
		virtual void set_parity(const uart_parity parity) = 0;
		virtual void set_flow_control(const uart_flow_control flow_control) = 0;
		virtual size_t get_baud_rate() const = 0;
		virtual size_t get_data_bits() const = 0;
		virtual size_t get_stop_bits() const = 0;
		virtual uart_parity get_parity() const = 0;
		virtual uart_flow_control get_flow_control() const = 0;
	};
}
