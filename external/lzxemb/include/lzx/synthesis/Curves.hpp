#pragma once

#include <cmath>
#include <lzx/synthesis/curve.hpp>

namespace lzx
{

    class curves
    {
    public:
        static const inline double PI = 3.141592653589793238463;

        template <typename T>
        static inline T get(const T value, const curve curve = curve::kLinear)
        {
            switch (curve)
            {
            case curve::kSaw:
                return getSaw(value);
            case curve::kSquare:
                return getSquare(value);
            case curve::kTriangle:
                return getTriangle(value);
            case curve::kSine:
                return getEaseInOutSine(getTriangle(value));
            case curve::kCosine:
                return getEaseInOutCosine(getTriangle(value));
            case curve::kEaseInSine:
                return getEaseInSine(value);
            case curve::kEaseOutSine:
                return getEaseOutSine(value);
            case curve::kEaseInOutSine:
                return getEaseInOutSine(value);
            case curve::kEaseInQuad:
                return getEaseInQuad(value);
            case curve::kEaseOutQuad:
                return getEaseOutQuad(value);
            case curve::kEaseInOutQuad:
                return getEaseInOutQuad(value);
            case curve::kEaseInCubic:
                return getEaseInCubic(value);
            case curve::kEaseOutCubic:
                return getEaseOutCubic(value);
            case curve::kEaseInOutCubic:
                return getEaseInOutCubic(value);
            case curve::kEaseInQuart:
                return getEaseInQuart(value);
            case curve::kEaseOutQuart:
                return getEaseOutQuart(value);
            case curve::kEaseInOutQuart:
                return getEaseInOutQuart(value);
            case curve::kEaseInQuint:
                return getEaseInQuint(value);
            case curve::kEaseOutQuint:
                return getEaseOutQuint(value);
            case curve::kEaseInOutQuint:
                return getEaseInOutQuint(value);
            case curve::kEaseInExpo:
                return getEaseInExpo(value);
            case curve::kEaseOutExpo:
                return getEaseOutExpo(value);
            case curve::kEaseInOutExpo:
                return getEaseInOutExpo(value);
            case curve::kEaseInCirc:
                return getEaseInCirc(value);
            case curve::kEaseOutCirc:
                return getEaseOutCirc(value);
            case curve::kEaseInOutCirc:
                return getEaseInOutCirc(value);
            case curve::kEaseInBack:
                return getEaseInBack(value);
            case curve::kEaseOutBack:
                return getEaseOutBack(value);
            case curve::kEaseInOutBack:
                return getEaseInOutBack(value);
            case curve::kEaseInElastic:
                return getEaseInElastic(value);
            case curve::kEaseOutElastic:
                return getEaseOutElastic(value);
            case curve::kEaseInOutElastic:
                return getEaseInOutElastic(value);
            case curve::kEaseInBounce:
                return getEaseInBounce(value);
            case curve::kEaseOutBounce:
                return getEaseOutBounce(value);
            case curve::kEaseInOutBounce:
                return getEaseInOutBounce(value);
            case curve::kLinear:
            default:
                return getLinear(value);
            };
        }

        template <typename T>
        static inline T getSquare(const T value)
        {
            return value > 0.5 ? 0 : 1;
        }

        template <typename T>
        static inline T getTriangle(const T value)
        {
            return 1 - std::abs((value * 2.0) - 1.0);
        }

        template <typename T>
        static inline T getEaseInCosine(const T value)
        {
            return 1 - std::sin((value * (T)PI) / 2);
        }

        template <typename T>
        static inline T getEaseOutCosine(const T value)
        {
            return std::cos((value * (T)PI) / 2);
        }

        template <typename T>
        static inline T getEaseInOutCosine(const T value)
        {
            return -(std::sin((T)PI * value) - 1) / 2;
        }

        template <typename T>
        static inline T getEaseInSine(const T value)
        {
            return 1 - std::cos((value * (T)PI) / 2);
        }

        template <typename T>
        static inline T getEaseOutSine(const T value)
        {
            return std::sin((value * (T)PI) / 2);
        }

        template <typename T>
        static inline T getEaseInOutSine(const T value)
        {
            return -(std::cos((T)PI * value) - 1) / 2;
        }

        template <typename T>
        static inline T getEaseInQuad(const T value)
        {
            return value * value;
        }

        template <typename T>
        static inline T getEaseOutQuad(const T value)
        {
            return 1 - (1 - value) * (1 - value);
        }

        template <typename T>
        static inline T getEaseInOutQuad(const T value)
        {
            return value < 0.5 ? 2 * value * value : 1 - std::pow(-2 * value + 2, 2) / 2;
        }

        template <typename T>
        static inline T getEaseInCubic(const T value)
        {
            return value * value * value;
        }

        template <typename T>
        static inline T getEaseOutCubic(const T value)
        {
            return 1 - std::pow(1 - value, 3);
        }

        template <typename T>
        static inline T getEaseInOutCubic(const T value)
        {
            return value < 0.5 ? 4 * value * value * value : 1 - std::pow(-2 * value + 2, 3) / 2;
        }

        template <typename T>
        static inline T getEaseInQuart(const T value)
        {
            return value * value * value * value;
        }

        template <typename T>
        static inline T getEaseOutQuart(const T value)
        {
            return 1 - std::pow(1 - value, 4);
        }

        template <typename T>
        static inline T getEaseInOutQuart(const T value)
        {
            return value < 0.5 ? 8 * value * value * value * value : 1 - std::pow(-2 * value + 2, 4) / 2;
        }

        template <typename T>
        static inline T getEaseInQuint(const T value)
        {
            return value * value * value * value * value;
        }

        template <typename T>
        static inline T getEaseOutQuint(const T value)
        {
            return 1 - std::pow(1 - value, 5);
        }

        template <typename T>
        static inline T getEaseInOutQuint(const T value)
        {
            return value < 0.5 ? 16 * value * value * value * value * value : 1 - std::pow(-2 * value + 2, 5) / 2;
        }

        template <typename T>
        static inline T getEaseInExpo(const T value)
        {
            return value == 0 ? 0 : std::pow(2, 10 * value - 10);
        }

        template <typename T>
        static inline T getEaseOutExpo(const T value)
        {
            return value == 1 ? 1 : 1 - std::pow(2, -10 * value);
        }

        template <typename T>
        static inline T getEaseInOutExpo(const T value)
        {
            return value == 0 ? 0 : value == 1 ? 1
                                : value < 0.5  ? std::pow(2, 20 * value - 10) / 2
                                               : (2 - std::pow(2, -20 * value + 10)) / 2;
        }

        template <typename T>
        static inline T getEaseInCirc(const T value)
        {
            return 1 - std::sqrt(1 - std::pow(value, 2));
        }

        template <typename T>
        static inline T getEaseOutCirc(const T value)
        {
            return std::sqrt(1 - std::pow(value - 1, 2));
        }

        template <typename T>
        static inline T getEaseInOutCirc(const T value)
        {
            return value < 0.5
                       ? (1 - std::sqrt(1 - std::pow(2 * value, 2))) / 2
                       : (std::sqrt(1 - std::pow(-2 * value + 2, 2)) + 1) / 2;
        }

        template <typename T>
        static inline T getEaseInBack(const T value)
        {
            const T c1 = 1.70158;
            const T c3 = c1 + 1;
            return c3 * value * value * value - c1 * value * value;
        }

        template <typename T>
        static inline T getEaseOutBack(const T value)
        {
            const T c1 = 1.70158;
            const T c3 = c1 + 1;
            return 1 + c3 * std::pow(value - 1, 3) + c1 * std::pow(value - 1, 2);
        }

        template <typename T>
        static inline T getEaseInOutBack(const T value)
        {
            const T c1 = 1.70158;
            const T c2 = c1 * 1.525;
            return value < 0.5
                       ? (std::pow(2 * value, 2) * ((c2 + 1) * 2 * value - c2)) / 2
                       : (std::pow(2 * value - 2, 2) * ((c2 + 1) * (value * 2 - 2) + c2) + 2) / 2;
        }

        template <typename T>
        static inline T getEaseInElastic(const T value)
        {
            const T c4 = (2 * (T)PI) / 3;
            return value == 0
                       ? 0
                   : value == 1
                       ? 1
                       : -std::pow(2, 10 * value - 10) * std::sin((value * 10 - 10.75) * c4);
        }

        template <typename T>
        static inline T getEaseOutElastic(const T value)
        {
            const T c4 = (2 * (T)PI) / 3;
            return value == 0
                       ? 0
                   : value == 1
                       ? 1
                       : std::pow(2, -10 * value) * std::sin((value * 10 - 0.75) * c4) + 1;
        }

        template <typename T>
        static inline T getEaseInOutElastic(const T value)
        {
            const T c5 = (2 * (T)PI) / 4.5;
            return value == 0
                       ? 0
                   : value == 1
                       ? 1
                   : value < 0.5
                       ? -(std::pow(2, 20 * value - 10) * std::sin((20 * value - 11.125) * c5)) / 2
                       : (std::pow(2, -20 * value + 10) * std::sin((20 * value - 11.125) * c5)) / 2 + 1;
        }

        template <typename T>
        static inline T getEaseOutBounce(const T value)
        {
            const T n1 = 7.5625;
            const T d1 = 2.75;
            T tmp = value;

            if (tmp < 1 / d1)
            {
                return n1 * tmp * tmp;
            }
            else if (tmp < 2 / d1)
            {
                return n1 * (tmp -= 1.5 / d1) * tmp + 0.75;
            }
            else if (tmp < 2.5 / d1)
            {
                return n1 * (tmp -= 2.25 / d1) * tmp + 0.9375;
            }
            else
            {
                return n1 * (tmp -= 2.625 / d1) * tmp + 0.984375;
            }
        }

        template <typename T>
        static inline T getEaseInBounce(const T value)
        {
            return 1 - getEaseOutBounce<T>(1 - value);
        }

        template <typename T>
        static inline T getEaseInOutBounce(const T value)
        {
            return value < 0.5
                       ? (1 - getEaseOutBounce(1 - 2 * value)) / 2
                       : (1 + getEaseOutBounce(2 * value - 1)) / 2;
        }

        template <typename T>
        static inline T getLinear(const T value)
        {
            return value;
        }

        template <typename T>
        static inline T getSaw(const T value)
        {
            return 1 - value;
        }
    };
}
