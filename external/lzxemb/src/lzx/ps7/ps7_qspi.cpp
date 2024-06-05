#include "ps7_qspi.hpp"

namespace lzx
{

    ps7_qspi::ps7_qspi(const uint16_t dev_id) : m_init(false)
    {
//        XQspiPs_Config *config = XQspiPs_LookupConfig(dev_id);
//        if (config != nullptr)
//        {
//            m_init = !(XQspiPs_CfgInitialize(&m_QspiPs, config, config->BaseAddress));
//            if (m_init)
//            {
//                // ("QSPI Controller %d Init OK, BaseAddr: 0x%08X, InputClockHz: %d, ConnectionMode: %d", dev_id, config->BaseAddress, config->InputClockHz, config->ConnectionMode);
//                if (!(XQspiPs_SelfTest(&m_QspiPs)))
//                {
//                    // ("QSPI Controller %d Self Test Success", dev_id);
//                }
//                else
//                {
//                    // ("QSPI Controller %d Self Test Failure", dev_id);
//                }
//            }
//            else
//            {
//                // ("QSPI Controller %d Init Error", dev_id);
//            }
//        }
//        else
//        {
//            // ("QSPI Controller %d Config Lookup Error", dev_id);
//        }

         		/* ---------------------------------------------------------------------------- *
         		 * 									SpiConfig()									*
         		 * ---------------------------------------------------------------------------- *
         		 * Initialises the SPI driver by looking up the configuration in the config
         		 * table and then initialising it. Also sets the SPI serial clock rate.
         		 * ---------------------------------------------------------------------------- */
         		XQspiPs_Config *Config;

         		m_clockRate = XQSPIPS_CLK_PRESCALE_8;
         		m_deviceIdPS = dev_id;

         		// Initialise the SPI driver so that it's ready to use */

         		Config = XQspiPs_LookupConfig(m_deviceIdPS);
         		if (NULL == Config)
         		{
         			// DBG("No QSPI Flash Config Found");
         			return;
         		}

         		m_status = XQspiPs_CfgInitialize(&m_Qspi, Config,
         										 Config->BaseAddress);
         		if (m_status != XST_SUCCESS)
         		{
         			// DBG("QSPI Flash Config Initialization Failure");
         			return;
         		}

         		// Perform a self-test to check hardware build
         		m_status = XQspiPs_SelfTest(&m_Qspi);
         		if (m_status != XST_SUCCESS)
         		{
         			// DBG("QSPI Flash Self Test Failure");
         			return;
         		}

         		// Set the SPI device as a master with manual start and manual
         		// chip select mode options
         		m_status = XQspiPs_SetOptions(&m_Qspi, XQSPIPS_MANUAL_START_OPTION |
         												   XQSPIPS_FORCE_SSELECT_OPTION |
         												   XQSPIPS_HOLD_B_DRIVE_OPTION);
         		if (m_status != XST_SUCCESS)
         		{
         			// DBG("QSPI Flash Set Options Failure");
         			return;
         		}

         		// Set the SPI device pre-scalar to divide by 128
         		m_status = XQspiPs_SetClkPrescaler(&m_Qspi, m_clockRate);
         		if (m_status != XST_SUCCESS)
         		{
         			// DBG("QSPI Flash Set Prescaler Failure");
         			return;
         		}
         		FlashReadID();
         		// // DBG("QSPI Init OK");
    }

    bool ps7_qspi::is_manual_start() const
    {
        return !!(XQspiPs_IsManualStart(const_cast<XQspiPs*>(&m_QspiPs)));
    }

    bool ps7_qspi::is_manual_chip_select() const
    {
        return !!(XQspiPs_IsManualChipSelect(const_cast<XQspiPs*>(&m_QspiPs)));
    }

    void ps7_qspi::set_slave_idle_count(const uint32_t v)
    {
        XQspiPs_SetSlaveIdle(&m_QspiPs, v);
    }

    uint32_t ps7_qspi::get_slave_idle_count() const
    {
        return !!(XQspiPs_GetSlaveIdle(&m_QspiPs));
    }

    void ps7_qspi::set_tx_watermark(const uint32_t v)
    {
        XQspiPs_SetTXWatermark(&m_QspiPs, v);
    }

    uint32_t ps7_qspi::get_tx_watermark() const
    {
        return (uint32_t)XQspiPs_GetTXWatermark((&m_QspiPs));
    }

    void ps7_qspi::set_rx_watermark(const uint32_t v)
    {
        XQspiPs_SetRXWatermark(&m_QspiPs, v);
    }

    uint32_t ps7_qspi::get_rx_watermark() const
    {
        return (uint32_t)XQspiPs_GetRXWatermark((&m_QspiPs));
    }

    void ps7_qspi::enable()
    {
        XQspiPs_Enable((&m_QspiPs));
    }

    void ps7_qspi::disable()
    {
        XQspiPs_Disable((&m_QspiPs));
    }

    void ps7_qspi::set_lqspi_config_reg(const uint32_t v)
    {
        XQspiPs_SetLqspiConfigReg(&m_QspiPs, v);
    }

    uint32_t ps7_qspi::get_lqspi_config_reg() const
    {
        return XQspiPs_GetLqspiConfigReg((&m_QspiPs));
    }

    void ps7_qspi::reset()
    {
        XQspiPs_Reset(&m_QspiPs);
    }

    void ps7_qspi::abort()
    {
        XQspiPs_Abort(&m_QspiPs);
    }

    bool ps7_qspi::transfer(const uint8_t *sendbuf, uint8_t *const recvbuf, const uint32_t bytes)
    {
        return !!(XQspiPs_Transfer(&m_QspiPs, (u8 *)sendbuf, (u8 *)recvbuf, (u32)bytes));
    }

    bool ps7_qspi::polled_transfer(const uint8_t *sendbuf, uint8_t *const recvbuf, const uint32_t bytes)
    {
        return !!(XQspiPs_PolledTransfer(&m_QspiPs, (u8 *)sendbuf, (u8 *)recvbuf, (u32)bytes));
    }

    bool ps7_qspi::lqspi_read(uint8_t *const recvbuf, const uint32_t addr, const uint32_t bytes)
    {
        return !!(XQspiPs_LqspiRead(&m_QspiPs, (u8 *)recvbuf, (u32)addr, (u32)bytes));
    }

    bool ps7_qspi::set_slave_select()
    {
        return !!(XQspiPs_SetSlaveSelect(&m_QspiPs));
    }

    void ps7_qspi::set_status_handler(qspi_status_handler handler)
    {
        XQspiPs_SetStatusHandler(&m_QspiPs, (void *)&m_QspiPs, (XQspiPs_StatusHandler)handler);
    }

    void ps7_qspi::interrupt_handler()
    {
        XQspiPs_InterruptHandler(&m_QspiPs);
    }

    bool ps7_qspi::set_options(const uint32_t v)
    {
        return !!(XQspiPs_SetOptions(&m_QspiPs, v));
    }

    uint32_t ps7_qspi::get_options() const
    {
        return (uint32_t)XQspiPs_GetOptions(const_cast<XQspiPs*>(&m_QspiPs));
    }

    bool ps7_qspi::set_clock_prescaler(const uint8_t v)
    {
        return !!(XQspiPs_SetClkPrescaler(&m_QspiPs, v));
    }

    uint8_t ps7_qspi::get_clock_prescaler() const
    {
        return (uint32_t)XQspiPs_GetClkPrescaler(const_cast<XQspiPs*>(&m_QspiPs));
    }

    bool ps7_qspi::set_delays(const uint8_t delay_nss, const uint8_t delay_btwn, const uint8_t delay_after, const uint8_t delay_init)
    {
        return !!(XQspiPs_SetDelays(&m_QspiPs, delay_nss, delay_btwn, delay_after, delay_init));
    }

    void ps7_qspi::get_delays(uint8_t *const delay_nss, uint8_t *const delay_btwn, uint8_t *const delay_after, uint8_t *const delay_init) const
    {
        XQspiPs_GetDelays(const_cast<XQspiPs*>(&m_QspiPs), delay_nss, delay_btwn, delay_after, delay_init);
    }


     	 ps7_qspi::~ps7_qspi()
     	{
     	}

     	 void ps7_qspi::FlashWrite(uint8_t buffer[], uint32_t Address, uint32_t ByteCount, uint8_t Command)
     	{
     		// ScopeGuard guard{m_mutex};
     		uint8_t WriteEnableCmd = {kWRITE_ENABLE_CMD};
     		uint8_t ReadStatusCmd[] = {kREAD_STATUS_CMD, 0}; /* must send 2 bytes */
     		uint8_t FlashStatus[2];

     		/*
     		 * Send the write enable command to the FLASH so that it can be
     		 * written to, this needs to be sent as a seperate transfer before
     		 * the write
     		 */
     		XQspiPs_PolledTransfer(&m_Qspi, &WriteEnableCmd, NULL,
     							   sizeof(WriteEnableCmd));

     		/*
     		 * Setup the write command with the specified address and data for the
     		 * FLASH
     		 */
     		buffer[kCOMMAND_OFFSET] = Command;
     		buffer[kADDRESS_1_OFFSET] = (uint8_t)((Address & 0xFF0000) >> 16);
     		buffer[kADDRESS_2_OFFSET] = (uint8_t)((Address & 0xFF00) >> 8);
     		buffer[kADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);

     		/*
     		 * Send the write command, address, and data to the FLASH to be
     		 * written, no receive buffer is specified since there is nothing to
     		 * receive
     		 */
     		XQspiPs_PolledTransfer(&m_Qspi, buffer, NULL,
     							   ByteCount + kOVERHEAD_SIZE);

     		/*
     		 * Wait for the write command to the FLASH to be completed, it takes
     		 * some time for the data to be written
     		 */
     		while (1)
     		{
     			/*
     			 * Poll the status register of the FLASH to determine when it
     			 * completes, by sending a read status command and receiving the
     			 * status byte
     			 */
     			XQspiPs_PolledTransfer(&m_Qspi, ReadStatusCmd, FlashStatus,
     								   sizeof(ReadStatusCmd));

     			/*
     			 * If the status indicates the write is done, then stop waiting,
     			 * if a value of 0xFF in the status byte is read from the
     			 * device and this loop never exits, the device slave select is
     			 * possibly incorrect such that the device status is not being
     			 * read
     			 */
     			if ((FlashStatus[1] & 0x01) == 0)
     			{
     				break;
     			}
     		}
     	}

     	 void ps7_qspi::FlashRead(uint32_t Address, uint32_t ByteCount, uint8_t Command, uint8_t *buffer)
     	{
     		// ScopeGuard guard{m_mutex};
     		uint8_t WriteBuffer[4];
     		/*
     		 * Setup the write command with the specified address and data for the
     		 * FLASH
     		 */
     		WriteBuffer[kCOMMAND_OFFSET] = Command;
     		WriteBuffer[kADDRESS_1_OFFSET] = (uint8_t)((Address & 0xFF0000) >> 16);
     		WriteBuffer[kADDRESS_2_OFFSET] = (uint8_t)((Address & 0xFF00) >> 8);
     		WriteBuffer[kADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);

     		if ((Command == kFAST_READ_CMD) || (Command == kDUAL_READ_CMD) ||
     			(Command == kQUAD_READ_CMD))
     		{
     			ByteCount += kDUMMY_SIZE;
     		}
     		/*
     		 * Send the read command to the FLASH to read the specified number
     		 * of bytes from the FLASH, send the read command and address and
     		 * receive the specified number of bytes of data in the data buffer
     		 */
     		XQspiPs_PolledTransfer(&m_Qspi, WriteBuffer, buffer,
     							   ByteCount + kOVERHEAD_SIZE);
     	}

     	 void ps7_qspi::FlashErase(uint32_t Address, uint32_t ByteCount)
     	{
     		// ScopeGuard guard{m_mutex};
     		uint8_t WriteEnableCmd = {kWRITE_ENABLE_CMD};
     		uint8_t ReadStatusCmd[] = {kREAD_STATUS_CMD, 0}; /* must send 2 bytes */
     		uint8_t FlashStatus[2];
     		uint8_t buffer[kSEC_ERASE_SIZE];
     		uint32_t Sector;

     		XQspiPs_SetSlaveSelect(&m_Qspi);
     		/*
     		 * If erase size is same as the total size of the flash, use bulk erase
     		 * command
     		 */
     		if (ByteCount == (kNUM_SECTORS * kSECTOR_SIZE))
     		{
     			/*
     			 * Send the write enable command to the FLASH so that it can be
     			 * written to, this needs to be sent as a seperate transfer
     			 * before the erase
     			 */
     			XQspiPs_PolledTransfer(&m_Qspi, &WriteEnableCmd, NULL,
     								   sizeof(WriteEnableCmd));

     			/*
     			 * Setup the bulk erase command
     			 */
     			buffer[kCOMMAND_OFFSET] = kBULK_ERASE_CMD;

     			/*
     			 * Send the bulk erase command; no receive buffer is specified
     			 * since there is nothing to receive
     			 */
     			XQspiPs_PolledTransfer(&m_Qspi, buffer, NULL,
     								   kBULK_ERASE_SIZE);

     			/*
     			 * Wait for the erase command to the FLASH to be completed
     			 */
     			while (1)
     			{
     				/*
     				 * Poll the status register of the device to determine
     				 * when it completes, by sending a read status command
     				 * and receiving the status byte
     				 */
     				XQspiPs_PolledTransfer(&m_Qspi, ReadStatusCmd,
     									   FlashStatus,
     									   sizeof(ReadStatusCmd));

     				/*
     				 * If the status indicates the write is done, then stop
     				 * waiting; if a value of 0xFF in the status byte is
     				 * read from the device and this loop never exits, the
     				 * device slave select is possibly incorrect such that
     				 * the device status is not being read
     				 */
     				if ((FlashStatus[1] & 0x01) == 0)
     				{
     					break;
     				}
     			}

     			return;
     		}

     		/*
     		 * If the erase size is less than the total size of the flash, use
     		 * sector erase command
     		 */
     		for (Sector = 0; Sector < ((ByteCount + kSECTOR_SIZE - 1) / kSECTOR_SIZE); Sector++)
     		{
     			// // DBG("Using Sector Erase Command")
     			/*
     			 * Send the write enable command to the SEEPOM so that it can be
     			 * written to, this needs to be sent as a seperate transfer
     			 * before the write
     			 */
     			XQspiPs_PolledTransfer(&m_Qspi, &WriteEnableCmd, NULL,
     								   sizeof(WriteEnableCmd));

     			/*
     			 * Setup the write command with the specified address and data
     			 * for the FLASH
     			 */
     			buffer[kCOMMAND_OFFSET] = kSEC_ERASE_CMD;
     			buffer[kADDRESS_1_OFFSET] = (uint8_t)(Address >> 16);
     			buffer[kADDRESS_2_OFFSET] = (uint8_t)(Address >> 8);
     			buffer[kADDRESS_3_OFFSET] = (uint8_t)(Address & 0xFF);

     			/*
     			 * Send the sector erase command and address; no receive buffer
     			 * is specified since there is nothing to receive
     			 */
     			XQspiPs_PolledTransfer(&m_Qspi, buffer, NULL,
     								   kSEC_ERASE_SIZE);

     			/*
     			 * Wait for the sector erse command to the FLASH to be completed
     			 */
     			while (1)
     			{
     				/*
     				 * Poll the status register of the device to determine
     				 * when it completes, by sending a read status command
     				 * and receiving the status byte
     				 */
     				XQspiPs_PolledTransfer(&m_Qspi, ReadStatusCmd,
     									   FlashStatus,
     									   sizeof(ReadStatusCmd));

     				// // DBG("QSPIPollStatus, %d, %d", FlashStatus[0], FlashStatus[1]);

     				/*
     				 * If the status indicates the write is done, then stop
     				 * waiting, if a value of 0xFF in the status byte is
     				 * read from the device and this loop never exits, the
     				 * device slave select is possibly incorrect such that
     				 * the device status is not being read
     				 */
     				if ((FlashStatus[1] & 0x01) == 0)
     				{
     					// // DBG("QSPI Flash Erase Done");
     					break;
     				}
     			}

     			Address += kSECTOR_SIZE;
     		}
     	}

     	 int ps7_qspi::FlashReadID(void)
     	{
     		// ScopeGuard guard{m_mutex};
     		uint8_t WriteBuffer[kRD_ID_SIZE];
     		uint8_t ReadBuffer[kRD_ID_SIZE];

     		XQspiPs_SetSlaveSelect(&m_Qspi);

     		/*
     		 * Read ID in Auto mode.
     		 */
     		WriteBuffer[kCOMMAND_OFFSET] = kREAD_ID;
     		WriteBuffer[kADDRESS_1_OFFSET] = 0x23; /* 3 dummy bytes */
     		WriteBuffer[kADDRESS_2_OFFSET] = 0x08;
     		WriteBuffer[kADDRESS_3_OFFSET] = 0x09;

     		m_status = XQspiPs_PolledTransfer(&m_Qspi, WriteBuffer, ReadBuffer,
     										  kRD_ID_SIZE);
     		if (m_status != XST_SUCCESS)
     		{
     			return XST_FAILURE;
     		}

     		// xil_printf("FlashID=0x%x 0x%x 0x%x\n\r", ReadBuffer[1], ReadBuffer[2], ReadBuffer[3]);

     		return XST_SUCCESS;
     	}

}


