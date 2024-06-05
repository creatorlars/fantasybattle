#pragma once

#include <lzx/hardware/usb.hpp>
#include <xusbps.h>

namespace lzx
{

    class ps7_usb : public usb
    {
    public:
        ps7_usb(const uint16_t dev_id);
        void start();
        void stop();
        void force_full_speed();
        void start_timer_0(const uint32_t v);
        void stop_timer_0();
        void read_timer_0();
        void remote_wakeup();
        void enable_endpoint_in(const uint8_t endpoint_num);
        void enable_endpoint_out(const uint8_t endpoint_num);
        void disable_endpoint_in(const uint8_t endpoint_num);
        void disable_endpoint_out(const uint8_t endpoint_num);
        void stall_endpoint_in(const uint8_t endpoint_num);
        void stall_endpoint_out(const uint8_t endpoint_num);
        void unstall_endpoint_in(const uint8_t endpoint_num);
        void unstall_endpoint_out(const uint8_t endpoint_num);
        void flush_endpoint_in(const uint8_t endpoint_num);
        void flush_endpoint_out(const uint8_t endpoint_num);
        void enable_interrupts(const uint32_t mask);
        void disable_interrupts(const uint32_t mask);
        void enable_nak_interrupts(const uint32_t mask);
        void disable_nak_interrupts(const uint32_t mask);
        void clear_nak_interrupts(const uint32_t mask);
        void set_interrupt_threshold(const uint8_t mask);
        void set_setup_tripwire();
        void clear_setup_tripwire();
        void set_bits(const uint32_t reg, const uint32_t bits);
        void clear_bits(const uint32_t reg, const uint32_t bits);
        void interrupt_handler();
        void device_reset();
        void endpoint_buffer_release(const uint32_t handle);
        bool is_setup_tripwire_set() const;
        bool reset();
        bool bus_reset();
        bool set_device_address(const uint8_t addr);
        bool suspend();
        bool resume();
        bool request_host_resume();
        bool endpoint_buffer_send(const uint8_t endpoint_num, const uint8_t *buf, const size_t len);
        bool endpoint_buffer_send_with_zlt(const uint8_t endpoint_num, const uint8_t *buf, const uint32_t len);
        bool endpoint_buffer_receive(const uint8_t endpoint_num, uint8_t **buf, uint32_t *len, uint32_t *handle);
        bool endpoint_in_prime(const uint8_t endpoint_num);
        bool endpoint_out_prime(const uint8_t endpoint_num);
        bool endpoint_in_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref);
        bool endpoint_out_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref);
        bool endpoint_get_setup_data(const uint8_t endpoint_num, usb_endpoint_setup_data *setup);
        bool reconfigure_endpoint_in(usb_device_config *config, const uint8_t endpoint_num, int direction_changed);
        bool reconfigure_endpoint_out(usb_device_config *config, const uint8_t endpoint_num, int direction_changed);
        bool set_interrupt_handler(usb_interrupt_handler handler, const uint32_t mask);
        bool set_device_config(const usb_device_config *cfg);
        bool is_initialized() const;
        uint32_t get_base_address() const;
        uint16_t get_device_id() const;
        uint8_t get_max_endpoints() const;
        uint32_t get_frame_number() const;

    private:
        bool m_init;
        XUsbPs m_UsbPs;
    };


}
