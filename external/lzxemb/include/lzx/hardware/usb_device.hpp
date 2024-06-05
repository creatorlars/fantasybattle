#pragma once

#include <lzx/usb.hpp>

namespace lzx
{
    class usb_device
    {
    public:
		usb_device(usb& driver) : m_driver(driver) {}
        virtual ~usb_device() {}

        usb_device_config& config()
        {
        	return m_config;
        }

        const usb_device_config& config() const
        {
        	return m_config;
        }

        usb& driver()
        {
        	return m_driver;
        }

        const usb& driver() const
        {
        	return m_driver;
        }

    private:
        usb& m_driver;
        usb_device_config m_config;
    };
}
