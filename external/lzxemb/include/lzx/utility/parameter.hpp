#pragma once

#include <lzx/utility/keys.hpp>
#include <etl/limits.h>
#include <etl/vector.h>
#include <etl/string.h>
#include <etl/string_utilities.h>
#include <etl/variant.h>
#include <etl/delegate.h>
#include <lzx/utility/archive.hpp>

namespace lzx
{

    class parameter
    {
    public:
        static constexpr size_t max_identifier_size = 16;
        typedef etl::string<max_identifier_size> identifier_type;
        typedef etl::delegate<void(const parameter &)> callback_type;

        template <typename T>
        struct is_valid_value_type
        {
            static constexpr bool value = etl::is_same<T, float>::value || etl::is_same<T, int>::value || etl::is_same<T, bool>::value;
        };

        explicit parameter(const char *name, float value, float min, float max);
        explicit parameter(const char *name, float value, float min, float max, float step);
        explicit parameter(const char *name, float value, float min, float max, int step_divisions);
        explicit parameter(const char *name, int value, int min, int max);
        explicit parameter(const char *name, float value);
        explicit parameter(const char *name, int value);
        explicit parameter(const char *name, int value, const ikeys &list);
        explicit parameter(const char *name, bool value);
        explicit parameter(const char *name, bool value, const ikeys &list);
        explicit parameter(const parameter &other) = delete; // must define an identifier
        explicit parameter(parameter &&other);
        void operator=(const parameter &other);
        void operator=(parameter &&other);
        void set_callback(callback_type callback);
        void clear_callback();
        bool is_valid() const;
        void acquire_from(const parameter &other);
        float step() const;
        void save(archive &a) const;
        void load(const archive &a);
        bool has_keys() const;
        const char *key() const;
        const ikeys *keys() const;
        int find_index_of_key(const char *name) const;
        void set_key(const char *name);
        void set_initial();
        void set_min();
        void set_max();
        void set_next();
        void set_previous();
        const char *id() const;

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        bool contains() const
        {
            if (etl::holds_alternative<float_v>(m_value) && etl::is_same<T, float>::value)
            {
                return true;
            }
            else if (etl::holds_alternative<int_v>(m_value) && etl::is_same<T, int>::value)
            {
                return true;
            }
            else if (etl::holds_alternative<bool_v>(m_value) && etl::is_same<T, bool>::value)
            {
                return true;
            }
            return false;
        }

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        void set(T value)
        {
            if (contains<float>() && etl::is_same<T, float>::value)
            {
                float_v &current = etl::get<float_v>(m_value);
                float incoming = etl::clamp<float>(float(value), current.min, current.max);
                if (current.value != incoming)
                {
                    current.value = incoming;
                    m_callback.call_if(*this);
                }
            }
            else if (contains<int>() && etl::is_same<T, int>::value)
            {
                int_v &current = etl::get<int_v>(m_value);
                int incoming = etl::clamp<int>(int(value), current.min, current.max);
                if (current.value != incoming)
                {
                    current.value = incoming;
                    m_callback.call_if(*this);
                }
            }
            else if (contains<bool>() && etl::is_same<T, bool>::value)
            {
                bool_v &current = etl::get<bool_v>(m_value);
                bool incoming = bool(value);
                if (current.value != incoming)
                {
                    current.value = incoming;
                    m_callback.call_if(*this);
                }
            }
        }

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        T get() const
        {
            if (contains<float>() && etl::is_same<T, float>::value)
            {
                return (T)etl::get<float_v>(m_value).value;
            }
            else if ((contains<int>() && etl::is_same<T, int>::value) || (contains<int>() && etl::is_enum<T>::value))
            {
                return (T)etl::get<int_v>(m_value).value;
            }
            else if (contains<bool>() && etl::is_same<T, bool>::value)
            {
                return (T)etl::get<bool_v>(m_value).value;
            }
            return {};
        }

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        T min() const
        {
            if (contains<float>() && etl::is_same<T, float>::value)
            {
                return (T)etl::get<float_v>(m_value).min;
            }
            else if (contains<int>() && etl::is_same<T, int>::value)
            {
                return (T)etl::get<int_v>(m_value).min;
            }
            else if (contains<bool>() && etl::is_same<T, bool>::value)
            {
                return (T) false;
            }
            return {};
        }

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        T max() const
        {
            if (contains<float>() && etl::is_same<T, float>::value)
            {
                return (T)etl::get<float_v>(m_value).max;
            }
            else if (contains<int>() && etl::is_same<T, int>::value)
            {
                return (T)etl::get<int_v>(m_value).max;
            }
            else if (contains<bool>() && etl::is_same<T, bool>::value)
            {
                return (T) true;
            }
            return {};
        }

        template <typename T, typename etl::enable_if<is_valid_value_type<T>::value>::type * = nullptr>
        T initial() const
        {
            if (contains<float>() && etl::is_same<T, float>::value)
            {
                return (T)etl::get<float_v>(m_value).initial;
            }
            else if (contains<int>() && etl::is_same<T, int>::value)
            {
                return (T)etl::get<int_v>(m_value).initial;
            }
            else if (contains<bool>() && etl::is_same<T, bool>::value)
            {
                return (T)etl::get<bool_v>(m_value).initial;
            }
            return {};
        }

    protected:
        struct bool_v
        {
            bool initial;
            bool value;
            const ikeys *list;
        };

        struct float_v
        {
            float initial;
            float min;
            float max;
            float value;
            float step;
        };

        struct int_v
        {
            int initial;
            int min;
            int max;
            int value;
            const ikeys *list;
        };

        typedef etl::variant<etl::monostate, bool_v, float_v, int_v> value_type;
        identifier_type m_id;
        value_type m_value;
        callback_type m_callback;
    };

    inline void parameter::acquire_from(const parameter &other)
    {
        if (contains<float>() && other.contains<float>())
        {
            set<float>(other.get<float>());
        }
        else if (contains<int>() && other.contains<int>())
        {
            set<int>(other.get<int>());
        }
        else if (contains<bool>() && other.contains<bool>())
        {
            set<bool>(other.get<bool>());
        }
    }

    inline const char *parameter::id() const
    {
        return m_id.c_str();
    }

    inline void parameter::operator=(const parameter &other)
    {
        acquire_from(other);
    }

    inline void parameter::operator=(parameter &&other)
    {
        m_id = other.m_id;
        m_value = other.m_value;
        m_callback = other.m_callback;
    }

    inline void parameter::set_callback(callback_type callback)
    {
        m_callback = callback;
    }

    inline void parameter::clear_callback()
    {
        m_callback.clear();
    }

    inline bool parameter::is_valid() const
    {
        return !(etl::holds_alternative<etl::monostate>(m_value));
    }

    inline float parameter::step() const
    {
        if (contains<float>())
        {
            return etl::get<float_v>(m_value).step;
        }
        return {};
    }

    inline void parameter::save(archive &a) const
    {
        if (contains<float>())
        {
            a.save_float(m_id.c_str(), get<float>());
        }
        else if (contains<int>())
        {
            a.save_int(m_id.c_str(), get<int>());
        }
        else if (contains<bool>())
        {
            a.save_bool(m_id.c_str(), get<bool>());
        }
    }

    inline void parameter::load(const archive &a)
    {
        if (a.contains_key(m_id.c_str()))
        {
            if (contains<float>())
            {
                set<float>(a.load_float(m_id.c_str()));
            }
            else if (contains<int>())
            {
                set<int>(a.load_int(m_id.c_str()));
            }
            else if (contains<bool>())
            {
                set<bool>(a.load_bool(m_id.c_str()));
            }
        }
    }

    inline bool parameter::has_keys() const
    {
        if (contains<bool>())
        {
            const bool_v &current = etl::get<bool_v>(m_value);
            return current.list != nullptr;
        }
        else if (contains<int>())
        {
            const int_v &current = etl::get<int_v>(m_value);
            return current.list != nullptr;
        }
        return false;
    }

    inline const char *parameter::key() const
    {
        if (contains<bool>())
        {
            const bool_v &current = etl::get<bool_v>(m_value);
            if (current.list != nullptr)
            {
                return current.list->at(int(current.value));
            }
        }
        else if (contains<int>())
        {
            const int_v &current = etl::get<int_v>(m_value);
            if (current.list != nullptr)
            {
                return current.list->at(current.value);
            }
        }
        return nullptr;
    }

    inline const ikeys *parameter::keys() const
    {
        if (contains<bool>())
        {
            const bool_v &current = etl::get<bool_v>(m_value);
            return current.list;
        }
        else if (contains<int>())
        {
            const int_v &current = etl::get<int_v>(m_value);
            return current.list;
        }
        return nullptr;
    }

    inline int parameter::find_index_of_key(const char *name) const
    {
        const ikeys *ls = keys();
        if (ls != nullptr)
        {
            return ls->find_index(name);
        }
        return {};
    }

    inline void parameter::set_key(const char *name)
    {
        if (has_keys() && contains<int>())
        {
            set<int>(find_index_of_key(name));
        }
        else if (has_keys() && contains<bool>())
        {
            set<bool>(find_index_of_key(name) != 0);
        }
    }

    inline void parameter::set_initial()
    {
        if (contains<float>())
        {
            set<float>(initial<float>());
        }
        else if (contains<int>())
        {
            set<int>(initial<int>());
        }
        else if (contains<bool>())
        {
            set<bool>(initial<bool>());
        }
    }

    inline void parameter::set_min()
    {
        if (contains<bool>())
        {
            set<bool>(min<bool>());
        }
        else if (contains<float>())
        {
            set<float>(min<float>());
        }
        else if (contains<int>())
        {
            set<int>(min<int>());
        }
    }

    inline void parameter::set_max()
    {
        if (contains<bool>())
        {
            set<bool>(min<bool>());
        }
        else if (contains<float>())
        {
            set<float>(min<float>());
        }
        else if (contains<int>())
        {
            set<int>(min<int>());
        }
    }

    inline void parameter::set_next()
    {
        if (contains<bool>())
        {
            set<bool>(!get<bool>());
        }
        else if (contains<float>())
        {
            set<float>(get<float>() + etl::get<float_v>(m_value).step);
        }
        else if (contains<int>() && has_keys())
        {
            if (get<int>() == max<int>())
            {
                set<int>(min<int>());
            }
            else
            {
                set<int>(get<int>() + int(1));
            }
        }
        else if (contains<int>() && !has_keys())
        {
            set<int>(get<int>() + int(1));
        }
    }

    inline void parameter::set_previous()
    {
        if (contains<bool>())
        {
            set<bool>(!get<bool>());
        }
        else if (contains<float>())
        {
            set<float>(get<float>() - etl::get<float_v>(m_value).step);
        }
        else if (contains<int>() && has_keys())
        {
            if (get<int>() == min<int>())
            {
                set<int>(max<int>());
            }
            else
            {
                set<int>(get<int>() - int(1));
            }
        }
        else if (contains<int>() && !has_keys())
        {
            set<int>(get<int>() - int(1));
        }
    }

    inline parameter::parameter(const char *name, float value, float min, float max) : m_id(name), m_value(etl::in_place_type<float_v>, float_v{value, min, max, value, 0.01f})
    {
    }

    inline parameter::parameter(const char *name, float value, float min, float max, float step) : m_id(name), m_value(etl::in_place_type<float_v>, float_v{value, min, max, value, step})
    {
    }

    inline parameter::parameter(const char *name, float value, float min, float max, int step_divisions) : m_id(name), m_value(etl::in_place_type<float_v>, float_v{value, min, max, value, (max - min) / step_divisions})
    {
    }

    inline parameter::parameter(const char *name, int value, int min, int max) : m_id(name), m_value(etl::in_place_type<int_v>, int_v{value, min, max, value, nullptr})
    {
    }

    inline parameter::parameter(const char *name, float value) : m_id(name), m_value(etl::in_place_type<float_v>, float_v{value, etl::numeric_limits<float>::min(), etl::numeric_limits<float>::max(), value, 0.01f})
    {
    }

    inline parameter::parameter(const char *name, int value) : m_id(name), m_value(etl::in_place_type<int_v>, int_v{value, etl::numeric_limits<int>::min(), etl::numeric_limits<int>::max(), value, nullptr})
    {
    }

    inline parameter::parameter(const char *name, int value, const ikeys &list) : m_id(name), m_value(etl::in_place_type<int_v>, int_v{value, int(0), int(list.size() - 1), value, &list})
    {
    }

    inline parameter::parameter(const char *name, bool value) : m_id(name), m_value(etl::in_place_type<bool_v>, bool_v{value, value, nullptr})
    {
    }

    inline parameter::parameter(const char *name, bool value, const ikeys &list) : m_id(name), m_value(etl::in_place_type<bool_v>, bool_v{value, value, &list})
    {
    }

    inline parameter::parameter(parameter &&other) : m_id(other.id()), m_value(other.m_value), m_callback(other.m_callback) {}
}
