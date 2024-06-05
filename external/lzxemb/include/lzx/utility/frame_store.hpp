#pragma once

#include <lzx/platform.hpp>
#include <etl/vector.h>
#include <etl/map.h>
#include <etl/optional.h>
#include <etl/alignment.h>

namespace lzx
{

    template <size_t StorageSize, size_t MaxFrames = 256, size_t MaxReaders = 16, size_t MaxWriters = 4, size_t StorageAlignment = sizeof(uint64_t), size_t LineAlignment = StorageAlignment, size_t FrameAlignment = LineAlignment>
    class frame_buffer
    {
    public:
        void reset()
        {
            // on_will_try_reset
            if (m_runtime.is_initialized)
            {
                // on_will_reset
                deinit(m_runtime);
                // on_reset
            }
            else
            {
                // on_try_reset_failed
            }
        }

        bool is_valid(size_t id) const
        {
            if (m_runtime.is_initialized && (m_runtime.readers.contains(id) || m_runtime.writers.contains(id)))
            {
                return true;
            }
            return false;
        }

        void destroy(size_t id)
        {
            // on_will_try_destroy_id
            if (m_runtime.is_initialized)
            {
                // on_will_try_find_id_to_destroy
                if (m_runtime.readers.contains(id))
                {
                    // on_will_destroy_reader
                    m_runtime.readers.erase(id);
                    // on_reader_destroyed
                }
                else if (m_runtime.writers.contains(id))
                {
                    // on_will_destroy_writer
                    m_runtime.writers.erase(id);
                    // on_writer_destroyed
                }
                else
                {
                    // on_try_find_id_to_destroy_failed
                }
            }
            else
            {
                // on_try_destroy_id_failed
            }
        }

        etl::optional<size_t> create_reader()
        {
            // on_will_try_create_reader
            if (m_runtime.is_initialized && m_runtime.readers.available())
            {
                // on_will_create_reader
                m_runtime.readers.insert({m_runtime.id_count, {m_runtime.id_count, {}}});
                // on_reader_created

                // on_will_increment_id_count
                m_runtime.id_count++;
                // on_id_count_incremented

                return m_runtime.readers.at(m_runtime.id_count - 1);
            }
            // on_try_create_reader_failed
            return {};
        }

        etl::optional<size_t> create_writer()
        {
            // on_will_try_create_writer
            if (m_runtime.is_initialized && m_runtime.writers.available())
            {
                // on_will_create_writer
                m_runtime.writers.insert({m_runtime.id_count, {m_runtime.id_count, {}}});
                // on_writer_created

                // on_will_increment_id_count
                m_runtime.id_count++;
                // on_id_count_incremented

                return m_runtime.writers.at(m_runtime.id_count - 1);
            }
            // on_try_create_writer_failed
            return {};
        }

        void init(size_t bytes_per_pixel, size_t pixels_per_line, size_t lines_per_frame)
        {
            // on_will_try_init
            if (!m_runtime.is_initialized)
            {
                // on_will_init
                m_config.bytes_per_pixel = bytes_per_pixel;
                m_config.pixels_per_line = pixels_per_line;
                m_config.lines_per_frame = lines_per_frame;
                init(m_buffer, m_storage);
                init(m_buffer, m_config, m_static);
                init(m_runtime, m_buffer, m_config);
                // on_init
            }
            else
            {
                // on_try_init_failed
            }
        }

        template <typename PixelType>
        void init(size_t pixels_per_line, size_t lines_per_frame)
        {
            init(sizeof(PixelType), pixels_per_line, lines_per_frame);
        }

    private:
        typedef etl::aligned_storage_t<StorageSize, StorageAlignment> storage_type;

        struct static_context
        {
            size_t storage_size = StorageSize;
            size_t storage_alignment = StorageAlignment;
            size_t line_alignment = LineAlignment;
            size_t frame_alignment = FrameAlignment;
            size_t max_frames = MaxFrames;
        };

        struct config_context
        {
            size_t bytes_per_pixel{0};
            size_t pixels_per_line{0};
            size_t lines_per_frame{0};
        };

        struct buffer_context
        {
            uint8_t *storage_address{nullptr};
            size_t bytes_per_line{0};
            size_t bytes_per_frame{0};
            size_t total_frames{0};
            size_t padding_bytes{StorageSize};
        };

        struct frame_context
        {
            size_t id{0};
            size_t width{0};
            size_t height{0};
            uint8_t *data{nullptr};
            etl::vector<size_t, MaxReaders> readers{};
            etl::vector<size_t, MaxWriters> writers{};
        };

        struct reader_context
        {
            size_t id{0};
            etl::optional<size_t> reading{};
        };

        struct writer_context
        {
            size_t id{0};
            etl::optional<size_t> writing{};
        };

        struct runtime_context
        {
            bool is_initialized{false};
            size_t life_cycle_count{0};
            size_t id_count{0};
            size_t frame_count{0};
            etl::vector<frame_context, MaxFrames> frames{};
            etl::map<size_t, reader_context, MaxReaders> readers{};
            etl::map<size_t, writer_context, MaxWriters> writers{};
        };

        static inline void deinit(runtime_context &runtime_ctx)
        {
            runtime_ctx.frames.clear();
            runtime_ctx.is_initialized = false;
        }

        void init(runtime_context &runtime_ctx, const buffer_context &buffer_ctx, const config_context &config_ctx)
        {
            if (!runtime_ctx.is_initialized)
            {
                runtime_ctx.frames.clear();

                while (!runtime_ctx.writers.empty())
                {
                    destroy(runtime_ctx.writers.at(runtime_ctx.writers.begin()->first).id);
                }

                while (!runtime_ctx.readers.empty())
                {
                    destroy(runtime_ctx.readers.at(runtime_ctx.readers.begin()->first).id);
                }

                runtime_ctx.writers.clear();

                for (size_t it = 0; it < buffer_ctx.total_frames; it++)
                {
                    // on_will_create_frame
                    runtime_ctx.frames.push_back({it, config_ctx.pixels_per_line, config_ctx.lines_per_frame, buffer_ctx.storage_address + (it * buffer_ctx.bytes_per_frame), {}, {}});
                    // on_frame_created
                }

                runtime_ctx.frame_count = 0;
                runtime_ctx.life_cycle_count++;
                runtime_ctx.is_initialized = true;
            }
        }

        static inline void init(buffer_context &buffer_ctx, const config_context &config_ctx, const static_context &static_ctx)
        {
            buffer_ctx.bytes_per_line = config_ctx.bytes_per_pixel * config_ctx.pixels_per_line;

            while ((buffer_ctx.bytes_per_line % static_ctx.line_alignment) != 0)
            {
                buffer_ctx.bytes_per_line++;
            }

            buffer_ctx.bytes_per_frame = buffer_ctx.bytes_per_line * config_ctx.lines_per_frame;

            while ((buffer_ctx.bytes_per_frame % static_ctx.frame_alignment) != 0)
            {
                buffer_ctx.bytes_per_frame++;
            }

            if ((static_ctx.storage_size / buffer_ctx.bytes_per_frame) >= static_ctx.max_frames)
            {
                buffer_ctx.total_frames = static_ctx.max_frames;
            }
            else
            {
                buffer_ctx.total_frames = static_ctx.storage_size / buffer_ctx.bytes_per_frame;
            }

            buffer_ctx.padding_bytes = static_ctx.storage_size - (buffer_ctx.total_frames * buffer_ctx.bytes_per_frame);
        }

        static inline void init(buffer_context &buffer_ctx, storage_type &storage)
        {
            buffer_ctx.storage_address = storage.get_address<uint8_t>();
        }

        storage_type m_storage;
        const static_context m_static;
        runtime_context m_runtime;
        config_context m_config;
        buffer_context m_buffer;
    };

    // frame_buffer()
    //     : m_buffer_address(buffer_address),
    //       m_buffer_size_bytes(buffer_size_bytes),
    //       m_pixel_size_bytes(0),
    //       m_buffer_size_pixels(0),
    //       m_line_size_pixels(0),
    //       m_line_size_bytes(0),
    //       m_buffer_size_lines(0),
    //       m_frame_size_lines(0),
    //       m_frame_size_pixels(0),
    //       m_frame_size_bytes(0),
    //       m_buffer_size_frames(0),
    //       m_frame_count(0),
    //       m_writer_count(0),
    //       m_duplicate_count(0),
    //       m_blank_count(0),
    //       m_display_count(0),
    //       m_initialized(false)
    // {
    // }

    // void reset()
    // {
    //     if (m_initialized)
    //     {
    //         m_frame_count = 0;
    //         m_writer_count = 0;
    //         m_duplicate_count = 0;
    //         m_display_count = 0;
    //         m_reading.reset();
    //         m_writing.reset();
    //         m_written.clear();
    //         m_initialized = false;
    //     }
    // }

    // void init(size_t pixel_size_bytes, size_t line_size_pixels, size_t frame_size_lines)
    // {
    //     if (!m_initialized)
    //     {
    //         m_pixel_size_bytes = pixel_size_bytes;
    //         m_line_size_pixels = line_size_pixels;
    //         m_frame_size_lines = frame_size_lines;
    //         m_buffer_size_pixels = m_buffer_size_bytes / m_pixel_size_bytes;
    //         m_line_size_bytes = m_line_size_pixels * m_pixel_size_bytes;
    //         m_buffer_size_lines = m_buffer_size_bytes / m_line_size_bytes;
    //         m_frame_size_pixels = m_frame_size_lines * m_line_size_pixels;
    //         m_frame_size_bytes = m_frame_size_pixels * m_pixel_size_bytes;
    //         m_buffer_size_frames = m_frame_size_bytes / m_frame_size_bytes;
    //         m_initialized = true;
    //     }
    // }

    // void next_frame()
    // {
    //     if (m_initialized)
    //     {
    //         if (!m_written.empty())
    //         {
    //             if (!m_reading)
    //             {
    //                 // first frame
    //             }
    //             m_reading = m_written.front();
    //             m_written.erase(m_written.begin());
    //             m_display_count++;
    //         }
    //         else
    //         {
    //             if (m_reading)
    //             {
    //                 m_duplicate_count++;
    //             }
    //             else
    //             {
    //                 m_blank_count++;
    //             }
    //         }
    //         m_frame_count++;
    //     }
    // }

    // const uint8_t *get_reader_data() const
    // {
    //     if (m_initialized && m_reading.has_value())
    //     {
    //         return m_buffer_address + (m_frame_size_bytes * m_reading.value());
    //     }
    //     return nullptr;
    // }

    // uint8_t *const get_writer_data() const
    // {
    //     if (m_initialized && m_writing.has_value())
    //     {
    //         return m_buffer_address + (m_frame_size_bytes * m_writing.value());
    //     }
    //     return nullptr;
    // }

    // bool try_lock_writer()
    // {
    //     if (m_initialized && !m_writing.has_value())
    //     {
    //         if ((m_written.size() + m_reading.has_value()) < m_buffer_size_frames)
    //         {
    //             if (m_written.empty() && m_reading.has_value())
    //             {
    //                 m_writing = (m_reading.value() + 1) % m_buffer_size_frames;
    //             }
    //             else if (m_written.empty() && !m_reading.has_value())
    //             {
    //                 m_writing = 0;
    //             }
    //             else
    //             {
    //                 m_writing = (m_written.back() + 1) % m_buffer_size_frames;
    //             }
    //             return true;
    //         }
    //     }
    //     return false;
    // }

    // bool try_release_writer()
    // {
    //     if (m_initialized && m_writing.has_value() && m_written.available())
    //     {
    //         m_written.push_back(m_writing.value());
    //         m_writing.reset();
    //         m_writer_count++;
    //         return true;
    //     }
    //     return false;
    // }

    // bool is_writing() const
    // {
    //     return !!(m_writing);
    // }

    // bool is_reading() const
    // {
    //     return !!(m_reading);
    // }

    // private:

    //     struct frame_context
    //     {
    //     };

    // static constexpr size_t c_max_frames = 256;
    // uint8_t *const m_buffer_address;
    // const size_t m_buffer_size_bytes;
    // size_t m_pixel_size_bytes;
    // size_t m_buffer_size_pixels;
    // size_t m_line_size_pixels;
    // size_t m_line_size_bytes;
    // size_t m_buffer_size_lines;
    // size_t m_frame_size_lines;
    // size_t m_frame_size_pixels;
    // size_t m_frame_size_bytes;
    // size_t m_buffer_size_frames;
    // size_t m_frame_count;
    // size_t m_writer_count;
    // size_t m_duplicate_count;
    // size_t m_blank_count;
    // size_t m_display_count;
    // bool m_initialized;
    // etl::optional<size_t> m_reading;
    // etl::optional<size_t> m_writing;
    // etl::vector<size_t, c_max_frames> m_written;
    // };
}