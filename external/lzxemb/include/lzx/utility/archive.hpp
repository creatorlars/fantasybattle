#pragma once

namespace lzx
{
    class archive
    {
    public:
        virtual ~archive() {}
        virtual void save_float(const char *key, float value) = 0;
        virtual void save_int(const char *key, int value) = 0;
        virtual void save_bool(const char *key, bool value) = 0;
        virtual bool load_bool(const char *key) const = 0;
        virtual float load_float(const char *key) const = 0;
        virtual int load_int(const char *key) const = 0;
        virtual bool contains_key(const char *name) const = 0;
    };
}
