#pragma once

#include <lzx/utility/parameter.hpp>
#include <lzx/utility/identifier.hpp>
#include <etl/map.h>
#include <etl/pool.h>
#include <etl/vector.h>

namespace lzx
{

    template <size_t Size>
    class model
    {
    public:
        static constexpr size_t max_parameters = Size;
        typedef etl::pool<parameter, max_parameters> pool_type;
        typedef etl::map<size_t, parameter *, max_parameters> map_type;
        typedef etl::vector<parameter *, max_parameters> vector_type;

        parameter &operator[](size_t id)
        {
            return *m_map.at(id);
        }

        const parameter &operator[](size_t id) const
        {
            return *m_map.at(id);
        }

        parameter &at(size_t index)
        {
            return *m_vector.at(index);
        }

        const parameter &at(size_t index) const
        {
            return *m_vector.at(index);
        }

        parameter &find(const char *name)
        {
            return *m_map.at(identifier(name));
        }

        const parameter &find(const char *name) const
        {
            return *m_map.at(identifier(name));
        }

        bool contains(size_t id) const
        {
            return m_map.contains(id);
        }

        bool contains(const char *name) const
        {
            return contains((size_t) identifier(name));
        }

        template <typename... Args>
        parameter *create(const char *id, Args &&...args)
        {
            if (m_pool.available() && !contains(id))
            {
                auto ptr = m_pool.create(id, etl::forward<Args>(args)...);
                if (ptr != nullptr)
                {
                    m_map.insert({identifier(id), ptr});
                    m_vector.push_back(ptr);
                    return ptr;
                }
            }
            return nullptr;
        }

        size_t size() const
        {
            return m_vector.size();
        }

        void destroy(size_t id)
        {
            if (contains(id))
            {
                for (size_t it = 0; it < m_vector.size(); it++)
                {
                    if (m_vector.at(it) == m_map.at(id))
                    {
                        m_vector.erase(m_vector.begin() + it);
                    }
                }
                m_pool.destroy(m_map.at(id));
                m_map.erase(id);
            }
        }

        void destroy_all()
        {
            while (m_map.empty() == false)
            {
                destroy(m_map.begin());
            }
        }

    private:
        pool_type m_pool;
        map_type m_map;
        vector_type m_vector;
    };
}
