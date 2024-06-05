#include "ps7_sd.hpp"

namespace lzx
{

	void ps7_sd::initialize_disk()
	{
		if (!(m_SdPs.IsReady))
		{
			XSdPs_CardInitialize(&m_SdPs);
		}
	}

	bool ps7_sd::sync_disk()
	{
		return true;
	}

	bool ps7_sd::trim_disk()
	{
		return true;
	}

	bool ps7_sd::is_disk_initialized() const
	{
		return m_SdPs.IsReady;
	}

	bool ps7_sd::is_disk_write_protected() const
	{
		if (m_wp != nullptr)
		{
			return m_wp->read();
		}
		return false;
	}

	bool ps7_sd::is_disk_media_detected() const
	{
		if (m_cd != nullptr)
		{
			return m_cd->read();
		}
		return true;
	}

	size_t ps7_sd::get_sector_size() const
	{
		return 512;
	}

	size_t ps7_sd::get_block_size() const
	{
		return 512;
	}

	size_t ps7_sd::get_sector_count() const
	{
		return m_SdPs.SectorCount;
	}

	size_t ps7_sd::write_disk(const uint8_t *buf, const size_t sector, const size_t count)
	{
		uint32_t LocSector = sector;

		/* Convert LBA to byte address if needed */
		if (m_SdPs.HCS == 0U)
		{
			LocSector *= (uint32_t)XSDPS_BLK_SIZE_512_MASK;
		}

		if (!!(XSdPs_WritePolled(&m_SdPs, LocSector, count, buf)))
		{
			return count;
		}

		return 0;
	}

	size_t ps7_sd::read_disk(uint8_t *const buf, const size_t sector, const size_t count) const
	{
		uint32_t LocSector = sector;

		/* Convert LBA to byte address if needed */
		if (m_SdPs.HCS == 0U)
		{
			LocSector *= (uint32_t)XSDPS_BLK_SIZE_512_MASK;
		}

		if (!!(XSdPs_ReadPolled(const_cast<XSdPs *>(&m_SdPs), LocSector, count, buf)))
		{
			return count;
		}

		return 0;
	}

	bool ps7_sd::power_disk()
	{
		return true;
	}

	bool ps7_sd::lock_disk()
	{
		return false;
	}

	bool ps7_sd::eject_disk()
	{
		return false;
	}

	bool ps7_sd::format_disk()
	{
		return false;
	}

	mmc_sdc_type ps7_sd::mmc_sdc_get_type() const
	{
		switch (m_SdPs.CardType)
		{
		case XSDPS_CARD_SD:
			return mmc_sdc_type::sd;
		case XSDPS_CARD_MMC:
			return mmc_sdc_type::mmc;
		case XSDPS_CARD_SDIO:
			return mmc_sdc_type::sdio;
		case XSDPS_CARD_SDCOMBO:
			return mmc_sdc_type::sdcombo;
		case XSDPS_CHIP_EMMC:
			return mmc_sdc_type::emmc;
		default:
			return mmc_sdc_type::unknown;
		}
	}

	size_t ps7_sd::mmc_sdc_get_csd() const
	{
		return 0;
	}

	size_t ps7_sd::mmc_sdc_get_cid() const
	{
		return 0;
	}

	size_t ps7_sd::mmc_sdc_get_ocr() const
	{
		return 0;
	}

	size_t ps7_sd::mmc_sdc_get_sdstat() const
	{
		return 0;
	}

	bool ps7_sd::mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const
	{
		return true;
	}

	bool ps7_sd::mmc_sdc_write_isdio(const uint8_t *buf, const size_t size)
	{
		return true;
	}

	bool ps7_sd::mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask)
	{
		return true;
	}

	ps7_sd::ps7_sd(const uint16_t dev_id, const input_pin<bool> *card_detect, const input_pin<bool> *write_protect) : m_initialized(false), m_cd(card_detect), m_wp(write_protect), m_id(dev_id)
	{
		XSdPs_Config *config = XSdPs_LookupConfig(m_id);
		if (config != nullptr)
		{
			m_baseAddr = config->BaseAddress;
			m_initialized = !(XSdPs_CfgInitialize(&m_SdPs, config, config->BaseAddress));
			if (m_initialized)
			{
//				// ("SD Controller %d Init OK, BaseAddr: 0x%08X, InputClockHz: %d, CardDetect: %d, WriteProtect: %d, BusWidth: %d, BankNumber: %d, HasEMIO: %d, IsCacheCoherent: %d", m_id, config->BaseAddress, config->InputClockHz, config->CardDetect, config->WriteProtect, config->BusWidth, config->BankNumber, config->HasEMIO, config->IsCacheCoherent);
			}
			else
			{
//				// ("SD Controller %d Init Error", m_id);
				m_initialized = false;
			}
		}
		else
		{
//			// ("SD Controller %d Config Lookup Error", m_id);
		}
	}




}
