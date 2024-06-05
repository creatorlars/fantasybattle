#pragma once

#include <cmath>
#include <etl/algorithm.h>

namespace lzx
{
    template <typename T>
    struct vec2
    {
        T x;
        T y;
    };

    template <typename T>
    struct mat2
    {
        vec2<T> x;
        vec2<T> y;
    };

    template <typename T>
    struct vec3
    {
        T x;
        T y;
        T z;
    };

    template <typename T>
    struct mat3
    {
        vec3<T> x;
        vec3<T> y;
        vec3<T> z;
    };

    template <typename T>
    struct vec4
    {
        T x;
        T y;
        T z;
        T w;
    };

    template <typename T>
    struct mat4
    {
        vec4<T> x;
        vec4<T> y;
        vec4<T> z;
        vec4<T> w;
    };

    static const inline double PI = 3.141592653589793238463;

    template <typename T>
    inline T normalizephase(const T &in)
    {
        T value = in;

        while (value > (T)1)
        {
            value -= (T)1;
        }

        while (value < (T)0)
        {
            value += (T)1;
        }

        return value;
    }

    template <typename T>
    inline int phasereps(const T &current, const T& increment)
    {
        T value = current + increment;
        int reps = 0;

        while (value > (T)1)
        {
            value -= (T)1;
            reps += 1;
        }

        while (value < (T)0)
        {
            value += (T)1;
            reps += 1;
        }

        return reps;
    }

    template <typename T>
    inline T accumulate(const T &out, const T &inc)
    {
        return normalizephase(out + inc);
    }

    template <typename T>
    inline T scale(const T &in, const T &imin, const T &imax, const T &omin, const T &omax)
    {
        return ((in - imin) * ((omax - omin) / (imax - imin))) + omin;
    }

    template <typename T>
    inline T nullgap(const T &in, const T &threshold = (T)0.1, const T &max = (T)1)
    {
        if (in >= threshold)
        {
            return (in - threshold) * (max / (max - threshold));
        }
        else if (in <= -threshold)
        {
            return ((in + max) * (max / (max - threshold))) - max;
        }
        else
        {
            return 0;
        }
    }

    template <typename T>
    inline T clamp(const T &in, const T &min, const T &max)
    {
        T value = in;

        if (value < min)
        {
            value = min;
        }

        if (value > max)
        {
            value = max;
        }

        return value;
    }

    template <typename T>
    inline T interpolate(const T &a, const T &b, const T &ratio)
    {
        return (a * (1.0 - ratio)) + (b * ratio);
    }

    template <typename T>
    inline T absolute(const T &in)
    {
        return std::abs(in);
    }

    template <typename T>
    inline T cartesian2distance(const T &x, const T &y)
    {
        return (x * x) - (y * y);
    }

    template <typename T>
    inline T cartesian2radians(const T &x, const T &y)
    {
        return std::atan2(x, y);
    }

    template <typename T>
    inline T radians2degrees(const T &rad)
    {
        return rad * ((T)180 / PI);
    }

    template <typename T>
    inline T degrees2radians(const T &degrees)
    {
        return (degrees * PI) / (T)180;
    }

    template <typename T>
    inline T cartesian2degrees(const T &x, const T &y)
    {
        return radians2degrees(cartesian2radians(x, y));
    }

    template <typename T>
    inline vec2<T> polar2cartesian(const T &angle, const T &distance)
    {
        return {(distance * std::cos(degrees2radians(angle))), (distance * std::sin(degrees2radians(angle)))};
    }

    template <typename T>
    inline vec2<T> cartesian2polar(const T &x, const T &y)
    {
        return {cartesian2degrees(x, y), cartesian2distance(x, y)};
    }

    template <typename T>
    inline T unipolar2bipolar(const T &v, const T &scale = (T)1)
    {
        return (v * (T)2) - scale;
    }

    template <typename T>
    inline T bipolar2unipolar(const T &v, const T &scale = (T)1)
    {
        return (v + scale) / (T)2;
    }

    static constexpr int kExpoRepsMax = 8;

    template <typename T>
    inline T radians2phase(const T &v)
    {
        return v / (PI * (T)2);
    }

    template <typename T>
    inline T phase2radians(const T &v)
    {
        return v * (PI * (T)2);
    }

    template <typename T>
    inline T phase2sin(const T &v)
    {
        return bipolar2unipolar(std::sin(phase2radians(v)));
    }

    template <typename T>
    inline T phase2cos(const T &v)
    {
        return bipolar2unipolar(std::cos(phase2radians(v)));
    }

    template <typename T>
    inline T phase2tri(const T &v)
    {
        return (T)1.0 - absolute((v * (T)2) - (T)1);
    }

    template <typename T>
    inline T phaseshift90(const T &v)
    {
        return normalizephase(v + 0.25);
    }

    template <typename T>
    inline T phase2tri90(const T &v)
    {
        return phase2tri(phaseshift90(v));
    }

    template <typename T>
    inline T phase2asin(const T &v)
    {
        const T value = unipolar2bipolar(phase2tri(v));
        return bipolar2unipolar(value * absolute(value));
    }

    template <typename T>
    inline T phase2acos(const T &v)
    {
        const T value = unipolar2bipolar(phase2tri90(v));
        return bipolar2unipolar(value * absolute(value));
    }

    template <typename T>
    inline T phase2qtri90(const T &v)
    {
        return clamp<T>((phase2tri90(v) * 2.0) - 0.5, 0, 1);
    }

    template <typename T>
    inline T phase2qtri(const T &v)
    {
        return clamp<T>((phase2tri(v) * 2.0) - 0.5, 0, 1);
    }

    template <typename T>
    inline T phase2saw(const T &v)
    {
        return (T)1.0 - v;
    }

    template <typename T>
    inline T phase2square(const T &v)
    {
        return v >= (T)0.5 ? (T)0 : (T)1;
    }

    template <typename T>
    inline T expo(const T &v, const int &power = 2)
    {
        T result = v;
        for (auto it = 0; it < clamp<int>(power - 1, 1, kExpoRepsMax); it++)
        {
            result = result * absolute(v);
        }
        return result;
    }

    template <typename T>
     T curve(const T &value, const T &ratio, const int &power = 2)
    {
        //return interpolate<T>(expo<T>(value, power), log<T>(value, power), ratio);
    	return T{};
    }

    template <typename T>
    inline T midpointnullgap(const T &in, const T &threshold = (T)0.1, const T &max = (T)1)
    {
        const T v = unipolar2bipolar<T>(in, max);
        const T v2 = nullgap<T>(v, threshold, max);
        const T v3 = bipolar2unipolar<T>(v2, max);
        return v3;
    }

    template <typename T>
    inline T frequency2period(const T &in)
    {
        return (T)1 / in;
    }

    template <typename T>
    inline T period2frequency(const T &in)
    {
        return (T)1 / in;
    }

    template <typename T>
    inline vec2<T> affine(const T &x, const T &y, const T &x2x, const T &x2y, const T &y2y, const T &y2x, const T &zx, const T &zy)
    {
        return {(x * x2x) + (y * y2x) + zx, (x * x2y) + (y * y2y) + zy};
    }

    template <typename T>
    inline vec2<T> rotateradians(const T &x, const T &y, const T &rad)
    {
        return affine(x, y, std::cos(rad), std::sin(rad), std::cos(rad), -std::sin(rad), 0, 0);
    }

    template <typename T>
    inline vec2<T> rotatedegrees(const T &x, const T &y, const T &deg)
    {
        const T rad = degrees2radians(deg);
        return affine(x, y, std::cos(rad), std::sin(rad), std::cos(rad), -std::sin(rad), 0, 0);
    }

    template <typename T>
    inline vec2<T> rotatephase(const T &x, const T &y, const T &phase)
    {
        const T rad = phase2radians(phase);
        return affine(x, y, std::cos(rad), std::sin(rad), std::cos(rad), -std::sin(rad), 0, 0);
    }

    template <typename T>
    inline vec2<T> translate(const T &x, const T &y, const T &x2, const T &y2)
    {
        return affine(x, y, 1, 0, 1, 0, x2, y2);
    }

    inline void seedrandom(const int& seed)
    {
        srand(seed);
    }

    template <typename T>
    inline T random(const T &min, const T &max)
    {
        if (std::is_floating_point<T>())
        {
            static const int factor = 10000;
            const int value = rand() % (factor + 1);
            const T ratiof = (T)value / (T)factor;
            const T span = max - min;
            const T valuef = span * ratiof;
            return (valuef + min);
        }

        const int span = (int)max - (int)min;
        const int value = rand() % (span + 1);
        return ((T)value + min);
    }

}
