#pragma once

namespace lzx
{

    template <typename ClockType, typename DurationType = typename ClockType::duration, typename TimePointType = typename ClockType::time_point>
    class stopwatch
    {
    public:
        stopwatch()
            : m_stamp(ClockType::now())
        {
        }

        DurationType update()
        {
            auto t = ClockType::now();
            auto d = t - m_stamp;
            m_stamp = t;
            return d;
        }

        void reset()
        {
            m_stamp = ClockType::now();
        }

    private:
        TimePointType m_stamp;
    };

};
