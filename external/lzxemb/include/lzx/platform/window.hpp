#pragma once

namespace lzx
{
    class window
    {
    public:
        virtual ~window() {}
        virtual bool is_open() const = 0;
    };
}