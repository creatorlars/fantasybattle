#pragma once

#include <chrono>
#include <stdio.h>

#define LZXEMB_DEBUG
#define LZXEMB_PLATFORM_STD
#define LZXEMB_PLATFORM_CLOCK_COUNTS_PER_SECOND std::chrono::high_resolution_clock::period::den
#define LZXEMB_PRINTF printf
