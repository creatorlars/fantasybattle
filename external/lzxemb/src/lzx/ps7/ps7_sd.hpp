#pragma once

#include <lzx/hardware/disk.hpp>
#include <lzx/hardware/pin.hpp>
#include <xsdps.h>

namespace lzx
{
	class ps7_sd : public disk_sd_mmc
	{
	public:
		ps7_sd(const uint16_t handle, const input_pin<bool> *card_detect, const input_pin<bool> *write_protect);
		void initialize_disk();
		bool sync_disk();
		bool trim_disk();
		bool is_disk_initialized() const;
		bool is_disk_write_protected() const;
		bool is_disk_media_detected() const;
		size_t get_sector_size() const;
		size_t get_block_size() const;
		size_t get_sector_count() const;
		size_t write_disk(const uint8_t *buf, const size_t sector, const size_t count);
		size_t read_disk(uint8_t *const buf, const size_t sector, const size_t count) const;
		bool power_disk();
		bool lock_disk();
		bool eject_disk();
		bool format_disk();
		mmc_sdc_type mmc_sdc_get_type() const;
		size_t mmc_sdc_get_csd() const;
		size_t mmc_sdc_get_cid() const;
		size_t mmc_sdc_get_ocr() const;
		size_t mmc_sdc_get_sdstat() const;
		bool mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const;
		bool mmc_sdc_write_isdio(const uint8_t *buf, const size_t size);
		bool mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask);

	private:
		XSdPs m_SdPs;
		uintptr_t m_baseAddr;
		bool m_initialized;
		const input_pin<bool> *m_cd;
		const input_pin<bool> *m_wp;
		uint16_t m_id;
	};

}
