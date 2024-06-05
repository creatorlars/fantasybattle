#pragma once

#include <lzx/utility/archive.hpp>
#include <ArduinoJson.hpp>

namespace lzx
{

    class json_archive : public archive
    {
    public:
        typedef ArduinoJson::StaticJsonDocument<1024> json_document_type;
        void save_float(const char *key, float value);
        void save_int(const char *key, int value);
        void save_bool(const char *key, bool value);
        bool load_bool(const char *key) const;
        float load_float(const char *key) const;
        int load_int(const char *key) const;
        bool contains_key(const char *name) const;

    private:
        json_document_type m_json;
    };

    inline void json_archive::save_float(const char *key, float value)
    {
        m_json[key] = value;
    }

    inline void json_archive::save_int(const char *key, int value)
    {
        m_json[key] = value;
    }

    inline void json_archive::save_bool(const char *key, bool value)
    {
        m_json[key] = value;
    }

    inline bool json_archive::load_bool(const char *key) const
    {
        return {};
    }

    inline float json_archive::load_float(const char *key) const
    {
        return m_json[key];
    }

    inline int json_archive::load_int(const char *key) const
    {
        return m_json[key];
    }

    inline bool json_archive::contains_key(const char *name) const
    {
        return m_json.containsKey(name);
    }

}
