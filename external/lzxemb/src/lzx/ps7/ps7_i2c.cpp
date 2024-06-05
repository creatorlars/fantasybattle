#include "ps7_i2c.hpp"

namespace lzx
{

    bool ps7_i2c::probe_addr(uint8_t addr)
    {
        uint16_t devaddr = addr;
        uint8_t tx[1] = {0};
        int32_t status = 1;

        status = XIicPs_SetOptions(const_cast<XIicPs *>(&m_IicPs), XIICPS_REP_START_OPTION);
        while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
        {
        }

        status = XIicPs_MasterSendPolled(const_cast<XIicPs *>(&m_IicPs), tx, 1, devaddr);

        if (!m_IicPs.IsRepeatedStart)
        {
            while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
            {
            }
        }

        status = XIicPs_ClearOptions(const_cast<XIicPs *>(&m_IicPs), XIICPS_REP_START_OPTION);

        uint8_t rx[1] = {0};

        status = XIicPs_MasterRecvPolled(const_cast<XIicPs *>(&m_IicPs), rx, 1, devaddr);
        while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
        {
        }

        return status == 0;
    }

    void ps7_i2c::write_bytes(uint8_t i2cAddr, uint8_t *buf, size_t numBytes)
    {
        // LockGuard guard{m_mutex};

        unsigned char u8TxData[numBytes];
        //
        //			u8TxData[0] = u8RegAddr;
        //			u8TxData[1] = u8Data;

        for (uint32_t it = 0; it < numBytes; it++)
        {
            u8TxData[it] = buf[it];
        }

        // Wait until I2C bus is not busy
        while (XIicPs_BusIsBusy(&m_IicPs))
            ;
        m_status = XIicPs_MasterSendPolled(&m_IicPs, u8TxData, numBytes, i2cAddr);
        while (XIicPs_BusIsBusy(&m_IicPs))
            ;
        //	u32 BaseAddr = InstancePtr->Config.BaseAddress;
        //	u32 IntrStatusReg = XIicPs_ReadReg(BaseAddr, XIICPS_ISR_OFFSET);
        //	std::cout << std::hex << IntrStatusReg << std::endl;
        //			if (m_status != XST_SUCCESS)
        //			{
        //				std::cout << "IC IIC0 Write Reg Failure" << std::endl;
        //			}
        // std::cout << std::hex << "[" << (unsigned int)u8RegAddr << "] = " << (unsigned int)u8Data << std::endl;
    }
    void ps7_i2c::read_bytes(uint8_t i2cAddr, uint8_t *const buf, size_t numBytes)
    {
        uint8_t u8Data[numBytes];
        // Read Data
        m_status = XIicPs_MasterRecvPolled(&m_IicPs, u8Data, numBytes, i2cAddr);
        while (XIicPs_BusIsBusy(&m_IicPs))
            ;
        for (uint32_t it = 0; it < numBytes; it++)
        {
            buf[it] = u8Data[it];
        }
    }
    ps7_i2c::ps7_i2c(uint16_t dev_id, uint32_t clock_frq) : m_init(false)
    {
        XIicPs_Config *config = XIicPs_LookupConfig(dev_id);
        if (config != nullptr)
        {
            m_init = !(XIicPs_CfgInitialize(&m_IicPs, config, config->BaseAddress));
            if (m_init)
            {
                // ("I2C Controller %d Init OK, BaseAddr: 0x%08X, InputClockHz: %d", dev_id, config->BaseAddress, config->InputClockHz);
                if (!(XIicPs_SelfTest(&m_IicPs)))
                {
                    // ("I2C Controller %d Self Test Success", dev_id);
                    XIicPs_SetSClk(&m_IicPs, clock_frq);
                }
                else
                {
                    // ("I2C Controller %d Self Test Failure", dev_id);
                }
            }
            else
            {
                // ("I2C Controller %d Init Error", dev_id);
            }
        }
        else
        {
            // ("I2C Controller %d Config Lookup Error", dev_id);
        }
    }

    void ps7_i2c::write_reg(uint8_t device_address, uint8_t reg_address, uint8_t reg_value)
    {
        uint16_t devaddr = device_address;
        uint8_t tx[2] = {reg_address, reg_value};

        while (!!(XIicPs_BusIsBusy(&m_IicPs)))
        {
        }

        m_status = XIicPs_MasterSendPolled(&m_IicPs, tx, 2, devaddr);

        while (!!(XIicPs_BusIsBusy(&m_IicPs)))
        {
        }
    }

    uint8_t ps7_i2c::read_reg(uint8_t device_address, uint8_t reg_address) const
    {
        uint16_t devaddr = device_address;
        uint8_t tx[1] = {reg_address};

        XIicPs_SetOptions(const_cast<XIicPs *>(&m_IicPs), XIICPS_REP_START_OPTION);
        while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
        {
        }

        XIicPs_MasterSendPolled(const_cast<XIicPs *>(&m_IicPs), tx, 1, devaddr);

        if (!m_IicPs.IsRepeatedStart)
        {
            while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
            {
            }
        }

        XIicPs_ClearOptions(const_cast<XIicPs *>(&m_IicPs), XIICPS_REP_START_OPTION);

        uint8_t rx[1] = {0};

        XIicPs_MasterRecvPolled(const_cast<XIicPs *>(&m_IicPs), rx, 1, devaddr);
        while (!!(XIicPs_BusIsBusy(const_cast<XIicPs *>(&m_IicPs))))
        {
        }

        return rx[0];
    }

}

// void I2C::readBytes(uint8_t &i2cAddr, uint8_t &startAddr, uint8_t *const buf, const uint32_t &numBytes)
// {
//     // LockGuard guard{m_mutex};
//     unsigned char u8TxData[1];
//     u8TxData[0] = startAddr;

//     // Enable Repeated Start
//     m_status = XIicPs_SetOptions(&m_Iic, XIICPS_REP_START_OPTION);
//     // Wait until I2C bus is not busy
//     while (XIicPs_BusIsBusy(&m_Iic))
//         ;
//     // Send Sub Address
//     m_status = XIicPs_MasterSendPolled(&m_Iic, u8TxData, 1, i2cAddr);

//     // Don't wait if
//     if (!m_Iic.IsRepeatedStart)
//     {
//         while (XIicPs_BusIsBusy(&m_Iic))
//             ;
//     }
//     if (m_status != XST_SUCCESS)
//     {
//         // return (ErrorCode) XST_FAILURE;
//     }

//     // Disable Repeated Start
//     m_status = XIicPs_ClearOptions(&m_Iic, XIICPS_REP_START_OPTION);

//     uint8_t u8Data[numBytes];
//     // Read Data
//     m_status = XIicPs_MasterRecvPolled(&m_Iic, u8Data, numBytes, i2cAddr);
//     while (XIicPs_BusIsBusy(&m_Iic))
//         ;
//     for (uint32_t it = 0; it < numBytes; it++)
//     {
//         buf[it] = u8Data[it];
//     }

//     //			if (m_status != XST_SUCCESS) {
//     //				return (ErrorCode)XST_FAILURE;
//     //			}
//     //	std::cout << std::hex << "[" << (unsigned int)u8RegAddr << "] = ";
//     //	for(int i=0;i< size; i++)
//     //	{
//     //		std::cout << (unsigned int)u8Data[i] << ", ";
//     //	}
//     //	std::cout << std::endl;
//     // return (ErrorCode) XST_SUCCESS;
//     //			return u8Data;
// }
