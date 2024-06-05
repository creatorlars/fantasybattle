#pragma once

#include <lzxemb_config.hpp>
#include <etl/exception.h>
#include <etl/error_handler.h>

#define LZXEMB_DEFINE_EXCEPTION(x, y)                                                                \
    struct x : public etl::exception                                                                 \
    {                                                                                                \
        x(string_type file_name, numeric_type line_number) : exception(y, file_name, line_number) {} \
    }

namespace lzx
{
    LZXEMB_DEFINE_EXCEPTION(platform_error, "Platform: general error");
};
