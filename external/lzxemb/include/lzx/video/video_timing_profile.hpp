#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
    template <typename PeriodType, uint32_t ClocksPerLine, uint32_t LinesPerFrame, uint32_t ActiveWidth, uint32_t ActiveHeight, bool IsInterlaced>
    class video_timing_profile
    {
    public:
        typedef PeriodType period;
        typedef clock<period> pixel_clock;
        static constexpr double clocks_per_second = pixel_clock::frequency;
        static constexpr uint32_t clocks_per_line = ClocksPerLine;
        static constexpr uint32_t lines_per_frame = LinesPerFrame;
        static constexpr uint32_t active_width = ActiveWidth;
        static constexpr uint32_t active_height = ActiveHeight;
        static constexpr bool is_interlaced = IsInterlaced;
        static constexpr bool is_progressive = !is_interlaced;
        static constexpr uint32_t blank_width = clocks_per_line - active_width;
        static constexpr uint32_t blank_height = lines_per_frame - active_height;
        static constexpr uint32_t clocks_per_frame = clocks_per_line * lines_per_frame;
        static constexpr double lines_per_field = is_interlaced ? double(lines_per_frame) / 2.0 : double(lines_per_frame);
        static constexpr uint32_t clocks_per_field = uint32_t(clocks_per_line * lines_per_field);
        static constexpr double lines_per_second = clocks_per_second / clocks_per_line;
        static constexpr double frames_per_second = clocks_per_second / clocks_per_frame;
        static constexpr double fields_per_second = is_interlaced ? clocks_per_second / clocks_per_field : frames_per_second;
        typedef clock<std::ratio_multiply<std::ratio<period::den, period::num>, std::ratio<1, clocks_per_frame>>> fsync_clock;
        typedef clock<std::ratio_multiply<std::ratio<period::den, period::num>, std::ratio<1, clocks_per_field>>> vsync_clock;
        typedef clock<std::ratio_multiply<std::ratio<period::den, period::num>, std::ratio<1, clocks_per_line>>> hsync_clock;
    };

}
