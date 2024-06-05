#pragma once

#include <lzx/platform/error.hpp>

#define LZXEMB_ASSERT_ERROR(x, y) ETL_ASSERT(x, ETL_ERROR(y))
#define LZXEMB_ASSERT(x) LZXEMB_ASSERT_ERROR(x, lzx::platform_error)
