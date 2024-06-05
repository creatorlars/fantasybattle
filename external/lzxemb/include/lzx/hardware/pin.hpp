#pragma once

#include <lzx/platform.hpp>

namespace lzx
{

    template <typename T>
    class input_pin
    {
    public:
        virtual ~input_pin() {}
        virtual T read() const = 0;
    };

    template <typename T>
    class output_pin
    {
    public:
        virtual ~output_pin() {}
        virtual void write(const T) = 0;
    };

    template <typename T>
    class input_output_pin : public input_pin<T>, public output_pin<T>
    {
    public:
        virtual ~input_output_pin() {}
    };

}
