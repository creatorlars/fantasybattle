#pragma once

#include <lzx/platform/error.hpp>
#include <lzx/platform/assert.hpp>

#ifdef LZXEMB_DEBUG
#define LZXEMB_DBG(...) LZXEMB_PRINTF(__VA_ARGS__)
#define LZXEMB_DBG_ASSERT_ERROR(x, y) LZXEMB_ASSERT_ERROR(x, y)
#define LZXEMB_DBG_ASSERT(x) LZXEMB_ASSERT(x)
#else
#define LZXEMB_DBG(...) (__VA_ARGS__)
#define LZXEMB_DBG_ASSERT_ERROR(x, y) (x, y)
#define LZXEMB_DBG_ASSERT(x) (x)
#endif
