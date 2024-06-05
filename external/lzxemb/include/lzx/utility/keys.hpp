#pragma once

#include <etl/string.h>
#include <etl/array.h>

namespace lzx
{

    class ikeys
    {
    public:
        virtual ~ikeys() {}
        virtual int size() const = 0;
        virtual const char *at(int index) const = 0;
        virtual int find_index(const char *k) const = 0;
    };

    template <int ListSize, int KeySize = 16>
    class keys : public ikeys
    {
    public:
        template <typename... Args>
        constexpr keys(Args &&...args) : m_data({etl::forward<Args>(args)...})
        {
        }

        constexpr int size() const
        {
            return ListSize;
        }

        constexpr const char *at(int index) const
        {
            return m_data[index].c_str();
        }

        constexpr int find_index(const char *k) const
        {
            for (int it = 0; it < (int)m_data.size(); it++)
            {
                if (strcmp(k, m_data[it].c_str()) == 0)
                {
                    return it;
                }
            }
            return -1;
        }

    private:
        etl::array<etl::string<KeySize>, ListSize> m_data;
    };

}
