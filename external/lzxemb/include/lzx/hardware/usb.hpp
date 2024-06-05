#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
    using usb_interrupt_handler = void (*)(void *cbref, uint32_t irq_mask);
    using usb_endpoint_handler = void (*)(void *cbref, uint8_t endpoint_num, uint8_t event_type, void *data);

    struct usb_endpoint_setup_data
    {
        uint8_t bmRequestType;
        uint8_t bRequest;
        uint16_t wValue;
        uint16_t wIndex;
        uint16_t wLength;
    };

    struct usb_endpoint_setup
    {
        uint32_t Type;
        uint32_t NumBufs;
        uint32_t BufSize;
        uint16_t MaxPacketSize;
    };

    struct usb_endpoint_config
    {
        usb_endpoint_setup Out;
        usb_endpoint_setup In;
    };

    struct usb_endpoint_out
    {
        uint8_t *dQH;
        uint8_t *dTDs;
        uint8_t *dTDCurr;
        uint8_t *dTDBufs;
        usb_endpoint_handler HandlerFunc;
        void *HandlerRef;
    };

    struct usb_endpoint_in
    {
        uint8_t *dQH;
        uint8_t *dTDs;
        uint8_t *dTDHead;
        uint8_t *dTDTail;
        usb_endpoint_handler HandlerFunc;
        void *HandlerRef;
    };

    struct usb_endpoint
    {
        usb_endpoint_out Out;
        usb_endpoint_in In;
    };

    struct usb_device_config
    {
        static constexpr uint8_t MaxEndpoints = 12;
        uint8_t NumEndpoints;
        usb_endpoint_config EpCfg[MaxEndpoints];
        uint32_t DMAMemPhys;
        usb_endpoint Ep[MaxEndpoints];
        uint32_t PhysAligned;
    };

    class usb
    {
    public:
        virtual ~usb() {}
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual void force_full_speed() = 0;
        virtual void start_timer_0(const uint32_t v) = 0;
        virtual void stop_timer_0() = 0;
        virtual void read_timer_0() = 0;
        virtual void remote_wakeup() = 0;
        virtual void enable_endpoint_in(const uint8_t endpoint_num) = 0;
        virtual void enable_endpoint_out(const uint8_t endpoint_num) = 0;
        virtual void disable_endpoint_in(const uint8_t endpoint_num) = 0;
        virtual void disable_endpoint_out(const uint8_t endpoint_num) = 0;
        virtual void stall_endpoint_in(const uint8_t endpoint_num) = 0;
        virtual void stall_endpoint_out(const uint8_t endpoint_num) = 0;
        virtual void unstall_endpoint_in(const uint8_t endpoint_num) = 0;
        virtual void unstall_endpoint_out(const uint8_t endpoint_num) = 0;
        virtual void flush_endpoint_in(const uint8_t endpoint_num) = 0;
        virtual void flush_endpoint_out(const uint8_t endpoint_num) = 0;
        virtual void enable_interrupts(const uint32_t mask) = 0;
        virtual void disable_interrupts(const uint32_t mask) = 0;
        virtual void enable_nak_interrupts(const uint32_t mask) = 0;
        virtual void disable_nak_interrupts(const uint32_t mask) = 0;
        virtual void clear_nak_interrupts(const uint32_t mask) = 0;
        virtual void set_interrupt_threshold(const uint8_t mask) = 0;
        virtual void set_setup_tripwire() = 0;
        virtual void clear_setup_tripwire() = 0;
        virtual void set_bits(const uint32_t reg, const uint32_t bits) = 0;
        virtual void clear_bits(const uint32_t reg, const uint32_t bits) = 0;
        virtual void interrupt_handler() = 0;
        virtual void device_reset() = 0;
        virtual void endpoint_buffer_release(const uint32_t handle) = 0;
        virtual bool is_setup_tripwire_set() const = 0;
        virtual bool reset() = 0;
        virtual bool bus_reset() = 0;
        virtual bool set_device_address(const uint8_t addr) = 0;
        virtual bool suspend() = 0;
        virtual bool resume() = 0;
        virtual bool request_host_resume() = 0;
        virtual bool endpoint_buffer_send(const uint8_t endpoint_num, const uint8_t *buf, const size_t len) = 0;
        virtual bool endpoint_buffer_send_with_zlt(const uint8_t endpoint_num, const uint8_t *buf, const uint32_t len) = 0;
        virtual bool endpoint_buffer_receive(const uint8_t endpoint_num, uint8_t **buf, uint32_t *len, uint32_t *handle) = 0;
        virtual bool endpoint_in_prime(const uint8_t endpoint_num) = 0;
        virtual bool endpoint_out_prime(const uint8_t endpoint_num) = 0;
        virtual bool endpoint_in_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref) = 0;
        virtual bool endpoint_out_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref) = 0;
        virtual bool endpoint_get_setup_data(const uint8_t endpoint_num, usb_endpoint_setup_data *setup) = 0;
        virtual bool reconfigure_endpoint_in(usb_device_config *config, const uint8_t endpoint_num, int direction_changed) = 0;
        virtual bool reconfigure_endpoint_out(usb_device_config *config, const uint8_t endpoint_num, int direction_changed) = 0;
        virtual bool set_interrupt_handler(usb_interrupt_handler handler, const uint32_t mask) = 0;
        virtual bool set_device_config(const usb_device_config *cfg) = 0;
        virtual bool is_initialized() const = 0;
        virtual uint32_t get_base_address() const = 0;
        virtual uint16_t get_device_id() const = 0;
        virtual uint8_t get_max_endpoints() const = 0;
        virtual uint32_t get_frame_number() const = 0;
    };
}
