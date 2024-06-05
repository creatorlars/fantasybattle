#pragma once

#include <lzx/disk.hpp>
#include <string.h>

namespace lzx
{

	class mem_disk : public disk_ram
	{
	public:
		static constexpr size_t sector_size = 512;
		static constexpr size_t block_size = 512;
		static constexpr size_t sector_count = 2048;

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

		mem_disk();
		~mem_disk();

	private:
		uint8_t m_data[sector_count][sector_size];

	};

	inline void mem_disk::initialize_disk()
	{
	}

	inline bool mem_disk::sync_disk()
	{
		return true;
	}

	inline bool mem_disk::trim_disk()
	{
		return true;
	}

	inline bool mem_disk::is_disk_initialized() const
	{
		return true;
	}

	inline bool mem_disk::is_disk_write_protected() const
	{
		return false;
	}

	inline bool mem_disk::is_disk_media_detected() const
	{
		return true;
	}

	inline size_t mem_disk::get_sector_size() const
	{
		return sector_size;
	}

	inline size_t mem_disk::get_block_size() const
	{
		return block_size;
	}

	inline size_t mem_disk::get_sector_count() const
	{
		return sector_count;
	}

	inline size_t mem_disk::write_disk(const uint8_t *buf, const size_t sector, const size_t count)
	{
		if(sector < sector_count && count < (sector_count - sector))
		{
			memcpy(&m_data[sector][0], buf, count * sector_size);
			return count;
		}
		return 0;
	}

	inline size_t mem_disk::read_disk(uint8_t *const buf, const size_t sector, const size_t count) const
	{
		if(sector < sector_count && count < (sector_count - sector))
		{
			memcpy(buf, &m_data[sector][0], count * sector_size);
			return count;
		}
		return 0;
	}

	inline bool mem_disk::power_disk()
	{
		return true;
	}

	inline bool mem_disk::lock_disk()
	{
		return false;
	}

	inline bool mem_disk::eject_disk()
	{
		return false;
	}

	inline bool mem_disk::format_disk()
	{
		return false;
	}

	inline mem_disk::mem_disk()
	{
		memset(&m_data[0][0], 0, sector_count * sector_size);
	}

	inline mem_disk::~mem_disk()
	{
	}

}
