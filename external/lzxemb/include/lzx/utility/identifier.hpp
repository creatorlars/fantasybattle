#pragma once

#include <lzx/platform.hpp>

namespace lzx
{
    namespace hashed_string
    {
        template <class>
        struct hasher;

        template <>
        struct hasher<const char *>
        {
            size_t constexpr operator()(char const *input) const
            {
                return *input ? static_cast<unsigned int>(*input) + 33 * (*this)(input + 1) : 5381;
            }
        };

        template <typename T>
        size_t constexpr hash(T &&t)
        {
            return hasher<typename etl::decay<T>::type>()(etl::forward<T>(t));
        }

    }

    class identifier
    {
    public:
        static constexpr size_t max_size = 32;
        constexpr identifier(const char *name) : m_name{0}, m_number(hashed_string::hasher<const char *>()(name))
        {
            for (auto it = 0; it < max_size - 1; it++)
            {
                if (name[it] == 0)
                {
                    break;
                }
                else
                {
                    m_name[it] = name[it];
                }
            }
        }

        constexpr operator const char *() const
        {
            return m_name;
        }

        constexpr operator size_t() const
        {
            return m_number;
        }

    private:
        char m_name[max_size];
        size_t m_number;
    };

    inline namespace literals
    {
        identifier constexpr operator"" _hs(const char *s, size_t)
        {
            return identifier(s);
        }
    }
}
