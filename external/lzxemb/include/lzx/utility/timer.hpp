#pragma once

namespace lzx
{

    template <typename DurationType, typename CounterType>
    class timer
    {
    public:
        timer(DurationType period)
            : m_delta(0),
              m_period(period)
        {
        }

        CounterType update(DurationType elapsed)
        {
            m_delta += elapsed;
            CounterType count = 0;
            while (m_delta >= m_period)
            {
                m_delta -= m_period;
                count++;
            }
            return count;
        }

        void set_delta(DurationType delta)
        {
            m_delta = delta;
        }

        void set_period(DurationType period)
        {
            m_period = period;
        }

        DurationType get_delta() const
        {
            return m_delta;
        }

        DurationType get_period() const
        {
            return m_period;
        }

        void reset()
        {
            set_delta(DurationType(0));
        }

    private:
        DurationType m_delta;
        DurationType m_period;
    };

};
