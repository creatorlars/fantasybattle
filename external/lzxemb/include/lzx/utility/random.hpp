#pragma once

#include <etl/random.h>

namespace lzx
{
    class random
    {
    public:
        typedef etl::random_xorshift generator_type;

        random(uint32_t seed) : m_gen(seed) {}
        
        template <typename T, typename etl::enable_if<etl::is_integral<T>::value>::type* = nullptr>
        T get(T min, T max) 
        {
            return T(m_gen.range((uint32_t) min, (uint32_t) max));
        }

        template <typename T, typename etl::enable_if<etl::is_floating_point<T>::value>::type* = nullptr>
        T get(T min, T max) 
        {
            T value = T(m_gen()) / T(UINT32_MAX);
            value*= (max - min);
            value+= min;
            return value;
        }

    private:
        generator_type m_gen;
    };
}
