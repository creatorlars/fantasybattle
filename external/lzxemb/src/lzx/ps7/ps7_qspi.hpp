#pragma once

#include <lzx/hardware/qspi.hpp>
#include <xqspips.h>

namespace lzx
{
    class ps7_qspi : public qspi
    {
    public:
        ps7_qspi(const uint16_t dev_id);
        bool is_manual_start() const;
        bool is_manual_chip_select() const;
        void set_slave_idle_count(const uint32_t v);
        uint32_t get_slave_idle_count() const;
        void set_tx_watermark(const uint32_t v);
        uint32_t get_tx_watermark() const;
        void set_rx_watermark(const uint32_t v);
        uint32_t get_rx_watermark() const;
        void enable();
        void disable();
        void set_lqspi_config_reg(const uint32_t v);
        uint32_t get_lqspi_config_reg() const;
        void reset();
        void abort();
        bool transfer(const uint8_t *sendbuf, uint8_t *const recvbuf, const uint32_t bytes);
        bool polled_transfer(const uint8_t *sendbuf, uint8_t *const recvbuf, const uint32_t bytes);
        bool lqspi_read(uint8_t *const recvbuf, const uint32_t addr, const uint32_t bytes);
        bool set_slave_select();
        void set_status_handler(qspi_status_handler handler);
        void interrupt_handler();
        bool set_options(const uint32_t v);
        uint32_t get_options() const;
        bool set_clock_prescaler(const uint8_t v);
        uint8_t get_clock_prescaler() const;
        bool set_delays(const uint8_t delay_nss, const uint8_t delay_btwn, const uint8_t delay_after, const uint8_t delay_init);
        void get_delays(uint8_t *const delay_nss, uint8_t *const delay_btwn, uint8_t *const delay_after, uint8_t *const delay_init) const;


 		void write(const uint32_t offset, uint8_t *buffer, const uint32_t size)
 		{
 			FlashReadID();
 			FlashWrite(buffer, offset, size, kWRITE_CMD);
 		}
 		void read(const uint32_t offset, uint8_t *const buffer, const uint32_t size)
 		{
 			FlashReadID();
 			FlashRead(offset, size, kREAD_CMD, buffer);
 		}

 		void erase(const uint32_t offset, const uint32_t size)
 		{
 			FlashReadID();
 			FlashErase(offset, size);
 		}

 		~ps7_qspi();

    private:
        bool m_init;
        XQspiPs m_QspiPs;

         		//	int Transfer(u32 chipSelect, u8 *u8TxData, u8 *u8RxData, u32 byteCount);
         		inline int GetStatus(void) { return m_status; };
         		//	inline void AddCS(io_pin* cs) {m_cs.push_back(cs);};
         		void FlashWrite(uint8_t buffer[], uint32_t Address, uint32_t ByteCount, uint8_t Command = kWRITE_CMD);
         		void FlashRead(uint32_t Address, uint32_t ByteCount, uint8_t Command, uint8_t *buffer);
         		void FlashErase(uint32_t Address, uint32_t ByteCount);
         		int FlashReadID(void);
         		XQspiPs m_Qspi;
         		unsigned int m_clockRate;
         		unsigned int m_deviceIdPS;
         		unsigned int m_status;

         		static const uint8_t kWRITE_STATUS_CMD = 0x01;
         		static const uint8_t kWRITE_CMD = 0x02;
         		static const uint8_t kREAD_CMD = 0x03;
         		static const uint8_t kWRITE_DISABLE_CMD = 0x04;
         		static const uint8_t kREAD_STATUS_CMD = 0x05;
         		static const uint8_t kWRITE_ENABLE_CMD = 0x06;
         		static const uint8_t kFAST_READ_CMD = 0x0B;
         		static const uint8_t kDUAL_READ_CMD = 0x3B;
         		static const uint8_t kQUAD_READ_CMD = 0x6B;
         		static const uint8_t kBULK_ERASE_CMD = 0xC7;
         		static const uint8_t kSEC_ERASE_CMD = 0xD8;
         		static const uint8_t kREAD_ID = 0x9F;
         		static const uint8_t kCOMMAND_OFFSET = 0;	/* FLASH instruction */
         		static const uint8_t kADDRESS_1_OFFSET = 1; /* MSB byte of address to read or write */
         		static const uint8_t kADDRESS_2_OFFSET = 2; /* Middle byte of address to read or write */
         		static const uint8_t kADDRESS_3_OFFSET = 3; /* LSB byte of address to read or write */
         		static const uint8_t kDATA_OFFSET = 4;		/* Start of Data for Read/Write */
         		static const uint8_t kDUMMY_OFFSET = 4;		/* Dummy byte offset for fast, dual and quad reads */
         		static const uint8_t kDUMMY_SIZE = 1;		/* Number of dummy bytes for fast, dual and quad reads */
         		static const uint8_t kRD_ID_SIZE = 4;		/* Read ID command + 3 bytes ID response */
         		static const uint8_t kBULK_ERASE_SIZE = 1;	/* Bulk Erase command size */
         		static const uint8_t kSEC_ERASE_SIZE = 4;	/* Sector Erase command + Sector address */
         		static const uint8_t kOVERHEAD_SIZE = 4;
         		static const uint32_t kSECTOR_SIZE = 0x10000;
         		static const uint32_t kNUM_SECTORS = 0x100;
         		static const uint32_t kNUM_PAGES = 0x10000;
         		static const uint32_t kPAGE_SIZE = 256;
         		static const uint32_t kPAGE_COUNT = 16;
         		static const uint32_t kTEST_ADDRESS = 0x00055000;
         		static const uint8_t kUNIQUE_VALUE = 0x05;
    };


}


