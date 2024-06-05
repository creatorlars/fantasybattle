#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
	enum class mmc_sdc_type : uint8_t
	{
		unknown = 0,
		sd,
		mmc,
		sdio,
		sdcombo,
		emmc
	};

	class disk
	{
	public:
		virtual ~disk() {}
		virtual void initialize_disk() = 0;
		virtual bool sync_disk() = 0;
		virtual bool trim_disk() = 0;
		virtual bool is_disk_initialized() const = 0;
		virtual bool is_disk_write_protected() const = 0;
		virtual bool is_disk_media_detected() const = 0;
		virtual size_t get_sector_size() const = 0;
		virtual size_t get_block_size() const = 0;
		virtual size_t get_sector_count() const = 0;
		virtual size_t write_disk(const uint8_t *buf, const size_t sector, const size_t count) = 0;
		virtual size_t read_disk(uint8_t *const buf, const size_t sector, const size_t count) const = 0;
		virtual bool power_disk() = 0;
		virtual bool lock_disk() = 0;
		virtual bool eject_disk() = 0;
		virtual bool format_disk() = 0;
		virtual bool is_disk_mmc_sdc() const = 0;
		virtual bool is_disk_ata_cf() const = 0;
		virtual bool is_disk_qspi_flash() const = 0;
		virtual bool is_disk_ram() const = 0;
		virtual mmc_sdc_type mmc_sdc_get_type() const = 0;
		virtual size_t mmc_sdc_get_csd() const = 0;
		virtual size_t mmc_sdc_get_cid() const = 0;
		virtual size_t mmc_sdc_get_ocr() const = 0;
		virtual size_t mmc_sdc_get_sdstat() const = 0;
		virtual bool mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const = 0;
		virtual bool mmc_sdc_write_isdio(const uint8_t *buf, const size_t size) = 0;
		virtual bool mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask) = 0;
		virtual size_t ata_cf_get_revision() const = 0;
		virtual size_t ata_cf_get_model() const = 0;
		virtual size_t ata_cf_get_serial_number() const = 0;

		size_t get_disk_size() const;
	};

	inline size_t disk::get_disk_size() const
	{
		return get_sector_size() * get_sector_count();
	}

	class disk_sd_mmc : public disk
	{
	public:
		virtual ~disk_sd_mmc() {}
		bool is_disk_mmc_sdc() const override { return true; }
		bool is_disk_ata_cf() const override { return false; }
		bool is_disk_qspi_flash() const override { return false; }
		bool is_disk_ram() const override { return false; }
		size_t ata_cf_get_revision() const { return 0; }
		size_t ata_cf_get_model() const { return 0; }
		size_t ata_cf_get_serial_number() const { return 0; }
	};

	class disk_ata_cf : public disk
	{
	public:
		virtual ~disk_ata_cf() {}
		bool is_disk_mmc_sdc() const override { return false; }
		bool is_disk_ata_cf() const override { return true; }
		bool is_disk_qspi_flash() const override { return false; }
		bool is_disk_ram() const override { return false; }
		mmc_sdc_type mmc_sdc_get_type() const { return mmc_sdc_type::unknown; }
		size_t mmc_sdc_get_csd() const { return 0; }
		size_t mmc_sdc_get_cid() const { return 0; }
		size_t mmc_sdc_get_ocr() const { return 0; }
		size_t mmc_sdc_get_sdstat() const { return 0; }
		bool mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const { return false; }
		bool mmc_sdc_write_isdio(const uint8_t *buf, const size_t size) { return false; }
		bool mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask) { return false; }
	};

	class disk_qspi_flash : public disk
	{
	public:
		virtual ~disk_qspi_flash() {}
		bool is_disk_mmc_sdc() const override { return false; }
		bool is_disk_ata_cf() const override { return false; }
		bool is_disk_qspi_flash() const override { return true; }
		bool is_disk_ram() const override { return false; }
		mmc_sdc_type mmc_sdc_get_type() const { return mmc_sdc_type::unknown; }
		size_t mmc_sdc_get_csd() const { return 0; }
		size_t mmc_sdc_get_cid() const { return 0; }
		size_t mmc_sdc_get_ocr() const { return 0; }
		size_t mmc_sdc_get_sdstat() const { return 0; }
		bool mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const { return false; }
		bool mmc_sdc_write_isdio(const uint8_t *buf, const size_t size) { return false; }
		bool mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask) { return false; }
		size_t ata_cf_get_revision() const { return 0; }
		size_t ata_cf_get_model() const { return 0; }
		size_t ata_cf_get_serial_number() const { return 0; }
	};

	class disk_ram : public disk
	{
	public:
		virtual ~disk_ram() {}
		bool is_disk_mmc_sdc() const override { return false; }
		bool is_disk_ata_cf() const override { return false; }
		bool is_disk_qspi_flash() const override { return false; }
		bool is_disk_ram() const override { return true; }
		mmc_sdc_type mmc_sdc_get_type() const { return mmc_sdc_type::unknown; }
		size_t mmc_sdc_get_csd() const { return 0; }
		size_t mmc_sdc_get_cid() const { return 0; }
		size_t mmc_sdc_get_ocr() const { return 0; }
		size_t mmc_sdc_get_sdstat() const { return 0; }
		bool mmc_sdc_read_isdio(uint8_t *const buf, const size_t size) const { return false; }
		bool mmc_sdc_write_isdio(const uint8_t *buf, const size_t size) { return false; }
		bool mmc_sdc_masked_write_isdio(const uint8_t *buf, const size_t size, const size_t mask) { return false; }
		size_t ata_cf_get_revision() const { return 0; }
		size_t ata_cf_get_model() const { return 0; }
		size_t ata_cf_get_serial_number() const { return 0; }
	};

}
