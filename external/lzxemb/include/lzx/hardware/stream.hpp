#pragma once

#include <lzx/platform.hpp>

namespace lzx
{

    template <typename T>
    class input_stream
    {
    public:
        virtual ~input_stream() {}
        virtual bool is_input_available() const = 0;
        virtual uint32_t read(T* const, const uint32_t) const = 0;
    };

    template <typename T>
    class output_stream
    {
    public:
        virtual ~output_stream() {}
        virtual bool is_output_available() const = 0;
        virtual uint32_t write(const T*, const uint32_t) = 0;
    };

    template <typename T>
    class input_output_stream : public input_stream<T>, public output_stream<T>
    {
    public:
        virtual ~input_output_stream() {}
    };

}
