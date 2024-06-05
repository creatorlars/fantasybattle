#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
	using qspi_status_handler = void (*) (void *, const uint32_t, const uint32_t);

    class qspi
    {
    public:
        virtual ~qspi() {}
        virtual bool is_manual_start() const = 0;
        virtual bool is_manual_chip_select() const = 0;
        virtual void set_slave_idle_count(const uint32_t v) = 0;
        virtual uint32_t get_slave_idle_count() const = 0;
        virtual void set_tx_watermark(const uint32_t v) = 0;
        virtual uint32_t get_tx_watermark() const = 0;
        virtual void set_rx_watermark(const uint32_t v) = 0;
        virtual uint32_t get_rx_watermark() const = 0;
        virtual void enable() = 0;
        virtual void disable() = 0;
        virtual void set_lqspi_config_reg(const uint32_t v) = 0;
        virtual uint32_t get_lqspi_config_reg() const = 0;
        virtual void reset() = 0;
        virtual void abort() = 0;
        virtual bool transfer(const uint8_t* sendbuf, uint8_t* const recvbuf, const uint32_t bytes) = 0;
        virtual bool polled_transfer(const uint8_t* sendbuf, uint8_t* const recvbuf, const uint32_t bytes) = 0;
        virtual bool lqspi_read(uint8_t* const recvbuf, const uint32_t addr, const uint32_t bytes) = 0;
        virtual bool set_slave_select() = 0;
        virtual void set_status_handler(qspi_status_handler handler) = 0;
        virtual void interrupt_handler() = 0;
        virtual bool set_options(const uint32_t v) = 0;
        virtual uint32_t get_options() const = 0;
        virtual bool set_clock_prescaler(const uint8_t v) = 0;
        virtual uint8_t get_clock_prescaler() const = 0;
        virtual bool set_delays(const uint8_t delay_nss, const uint8_t delay_btwn, const uint8_t delay_after, const uint8_t delay_init) = 0;
        virtual void get_delays( uint8_t* const delay_nss,  uint8_t* const delay_btwn,  uint8_t* const delay_after,  uint8_t* const delay_init) const = 0;

    };
}
