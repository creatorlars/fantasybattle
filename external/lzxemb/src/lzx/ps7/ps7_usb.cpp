#include "ps7_usb.hpp"

namespace lzx
{

    uint32_t ps7_usb::get_frame_number() const
    {
        return XUsbPs_GetFrameNum(&m_UsbPs);
    }

    void ps7_usb::start_timer_0(const uint32_t v)
    {
        XUsbPs_StartTimer0(&m_UsbPs, v);
    }

    void ps7_usb::read_timer_0()
    {
        XUsbPs_ReadTimer0(&m_UsbPs);
    }

    void ps7_usb::enable_endpoint_in(const uint8_t endpoint_num)
    {
        XUsbPs_EpEnable(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN);
    }

    void ps7_usb::enable_endpoint_out(const uint8_t endpoint_num)
    {
        XUsbPs_EpEnable(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT);
    }

    void ps7_usb::disable_endpoint_in(const uint8_t endpoint_num)
    {
        XUsbPs_EpDisable(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN);
    }

    void ps7_usb::disable_endpoint_out(const uint8_t endpoint_num)
    {
        XUsbPs_EpDisable(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT);
    }

    void ps7_usb::stall_endpoint_in(const uint8_t endpoint_num)
    {
        XUsbPs_EpStall(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN);
    }

    void ps7_usb::stall_endpoint_out(const uint8_t endpoint_num)
    {
        XUsbPs_EpStall(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT);
    }

    void ps7_usb::unstall_endpoint_in(const uint8_t endpoint_num)
    {
        XUsbPs_EpUnStall(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN);
    }

    void ps7_usb::unstall_endpoint_out(const uint8_t endpoint_num)
    {
        XUsbPs_EpUnStall(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT);
    }

    void ps7_usb::flush_endpoint_in(const uint8_t endpoint_num)
    {
        XUsbPs_EpFlush(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN);
    }

    void ps7_usb::flush_endpoint_out(const uint8_t endpoint_num)
    {
        XUsbPs_EpFlush(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT);
    }

    void ps7_usb::enable_interrupts(const uint32_t mask)
    {
        XUsbPs_IntrEnable(&m_UsbPs, mask);
    }

    void ps7_usb::disable_interrupts(const uint32_t mask)
    {
        XUsbPs_IntrDisable(&m_UsbPs, mask);
    }

    void ps7_usb::enable_nak_interrupts(const uint32_t mask)
    {
        XUsbPs_NakIntrEnable(&m_UsbPs, mask);
    }

    void ps7_usb::disable_nak_interrupts(const uint32_t mask)
    {
        XUsbPs_NakIntrDisable(&m_UsbPs, mask);
    }

    void ps7_usb::clear_nak_interrupts(const uint32_t mask)
    {
        XUsbPs_NakIntrClear(&m_UsbPs, mask);
    }

    void ps7_usb::set_interrupt_threshold(const uint8_t v)
    {
        XUsbPs_SetIntrThreshold(&m_UsbPs, v);
    }

    void ps7_usb::set_setup_tripwire()
    {
        XUsbPs_SetSetupTripwire(&m_UsbPs);
    }

    void ps7_usb::clear_setup_tripwire()
    {
        XUsbPs_ClrSetupTripwire(&m_UsbPs);
    }

    bool ps7_usb::is_setup_tripwire_set() const
    {
        return !!(XUsbPs_SetupTripwireIsSet(&m_UsbPs));
    }

    void ps7_usb::set_bits(const uint32_t reg, const uint32_t bits)
    {
        XUsbPs_SetBits(&m_UsbPs, reg, bits);
    }

    void ps7_usb::clear_bits(const uint32_t reg, const uint32_t bits)
    {
        XUsbPs_ClrBits(&m_UsbPs, reg, bits);
    }

    void ps7_usb::remote_wakeup()
    {
        XUsbPs_RemoteWakeup(&m_UsbPs);
    }

    void ps7_usb::stop_timer_0()
    {
        XUsbPs_StopTimer0(&m_UsbPs);
    }

    void ps7_usb::force_full_speed()
    {
        XUsbPs_ForceFS(&m_UsbPs);
    }

    void ps7_usb::start()
    {
        XUsbPs_Start(&m_UsbPs);
    }

    void ps7_usb::stop()
    {
        XUsbPs_Stop(&m_UsbPs);
    }

    void ps7_usb::interrupt_handler()
    {
        XUsbPs_IntrHandler(&m_UsbPs);
    }

    bool ps7_usb::reset()
    {
        return !!(XUsbPs_Reset(&m_UsbPs));
    }

    void ps7_usb::device_reset()
    {
        XUsbPs_DeviceReset(&m_UsbPs);
    }

    bool ps7_usb::bus_reset()
    {
        //    	return XUsbPs_BusReset(&m_UsbPs);
        return 0;
    }

    bool ps7_usb::set_device_address(const uint8_t addr)
    {
        return !!(XUsbPs_SetDeviceAddress(&m_UsbPs, addr));
    }

    bool ps7_usb::suspend()
    {
        return !!(XUsbPs_Suspend(&m_UsbPs));
    }

    bool ps7_usb::resume()
    {
        return !!(XUsbPs_Resume(&m_UsbPs));
    }

    bool ps7_usb::request_host_resume()
    {
        return !!(XUsbPs_RequestHostResume(&m_UsbPs));
    }

    bool ps7_usb::endpoint_buffer_send(const uint8_t endpoint_num, const uint8_t *buf, const size_t len)
    {
        return !!(XUsbPs_EpBufferSend(&m_UsbPs, endpoint_num, buf, len));
    }

    bool ps7_usb::endpoint_buffer_send_with_zlt(const uint8_t endpoint_num, const uint8_t *buf, const uint32_t len)
    {
        return !!(XUsbPs_EpBufferSendWithZLT(&m_UsbPs, endpoint_num, buf, len));
    }

    bool ps7_usb::endpoint_buffer_receive(const uint8_t endpoint_num, uint8_t **buf, uint32_t *len, uint32_t *handle)
    {
        return !!(XUsbPs_EpBufferReceive(&m_UsbPs, endpoint_num, buf, len, handle));
    }

    void ps7_usb::endpoint_buffer_release(const uint32_t handle)
    {
        XUsbPs_EpBufferRelease(handle);
    }

    bool ps7_usb::endpoint_in_prime(const uint8_t endpoint_num)
    {
        return !!(XUsbPs_EpPrime(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN));
    }

    bool ps7_usb::endpoint_out_prime(const uint8_t endpoint_num)
    {
        return !!(XUsbPs_EpPrime(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT));
    }

    uint8_t ps7_usb::get_max_endpoints() const
    {
        return XUSBPS_MAX_ENDPOINTS;
    }

    bool ps7_usb::endpoint_in_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref)
    {
        return !!(XUsbPs_EpSetHandler(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_IN, handler, cbref));
    }

    bool ps7_usb::endpoint_out_set_handler(const uint8_t endpoint_num, usb_endpoint_handler handler, void *cbref)
    {
        return !!(XUsbPs_EpSetHandler(&m_UsbPs, endpoint_num, XUSBPS_EP_DIRECTION_OUT, handler, cbref));
    }

    bool ps7_usb::endpoint_get_setup_data(const uint8_t endpoint_num, usb_endpoint_setup_data *setup)
    {
        return !!(XUsbPs_EpGetSetupData(&m_UsbPs, (int)endpoint_num, (XUsbPs_SetupData *)setup));
    }

    uint32_t ps7_usb::get_base_address() const
    {
        return m_UsbPs.Config.BaseAddress;
    }

    uint16_t ps7_usb::get_device_id() const
    {
        return m_UsbPs.Config.DeviceID;
    }

    bool ps7_usb::reconfigure_endpoint_in(usb_device_config *config, const uint8_t endpoint_num, int changed)
    {
        return !!(XUsbPs_ReconfigureEp(&m_UsbPs, (XUsbPs_DeviceConfig *)config, (uint8_t)endpoint_num, XUSBPS_EP_DIRECTION_IN, changed));
    }

    bool ps7_usb::reconfigure_endpoint_out(usb_device_config *config, const uint8_t endpoint_num, int changed)
    {
        return !!(XUsbPs_ReconfigureEp(&m_UsbPs, (XUsbPs_DeviceConfig *)config, (uint8_t)endpoint_num, XUSBPS_EP_DIRECTION_OUT, changed));
    }

    bool ps7_usb::set_interrupt_handler(usb_interrupt_handler handler, const uint32_t mask)
    {
        return !!(XUsbPs_IntrSetHandler(&m_UsbPs, handler, (void *)&m_UsbPs, mask));
    }

    bool ps7_usb::set_device_config(const usb_device_config *cfg)
    {
        return !!(XUsbPs_ConfigureDevice(&m_UsbPs, (const XUsbPs_DeviceConfig *)cfg));
    }

    bool ps7_usb::is_initialized() const
    {
        return m_init;
    }

    ps7_usb::ps7_usb(const uint16_t dev_id) : m_init(false)
    {
        const XUsbPs_Config *config = XUsbPs_LookupConfig(dev_id);
        if (config != nullptr)
        {
            m_init = !(XUsbPs_CfgInitialize(&m_UsbPs, config, config->BaseAddress));
            if (m_init)
            {
                // ("USB Controller %d Init OK, BaseAddr: 0x%08X", dev_id, config->BaseAddress);
            }
            else
            {
                // ("USB Controller %d Init Error", dev_id);
            }
        }
        else
        {
            // ("USB Controller %d Config Lookup Error", dev_id);
        }
    }


}
