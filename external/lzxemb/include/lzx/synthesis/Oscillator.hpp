#pragma once

#include <lzx/platform.hpp>
#include <lzx/utility/math.hpp>
#include <lzx/synthesis/parameter.hpp>

namespace lzx
{
    template <typename T>
    struct oscillator : public reactor
    {
    private:
    
    public:
        parameter<T> cv{*this};
        parameter<T> phasecv;
        parameter<T> randcv;
        parameter<bool> enable{*this};
        parameter<T> minFrequency{0.01};
        parameter<T> maxFrequency{10};
        parameter<T> clockFrequency{29.97};
        parameter<T> frequency{0.25};
        parameter<T> randsrc;
        parameter<T> randdst;
        parameter<T> randsrc2;
        parameter<T> randdst2;
        parameter<T> phase{*this};
        parameter<T> sine;
        parameter<T> cosine;
        parameter<T> asine;
        parameter<T> acosine;
        parameter<T> square;
        parameter<T> saw;
        parameter<T> tri;
        parameter<T> tri90;
        parameter<T> qtri;
        parameter<T> qtri90;
        parameter<T> phase90;
        parameter<T> randwave;
        parameter<T> randwave2;

        oscillator()
        {
            update();
        }

        void update()
        {
            frequency.set(interpolate<T>(minFrequency.get(), maxFrequency.get(), cv.get() * absolute(cv.get())));
//            DBG("CV: %d, Freq: %d", (uint32_t) (cv.get() * 1000), (uint32_t) (frequency.get() * 1000));
            sine.set(phase2sin(phase.get()));
            cosine.set(phase2sin(accumulate<T>(phase.get(), 0.25 + (0.25 * phasecv.get()))));
            asine.set(phase2asin(phase.get()));
            acosine.set(phase2cos(accumulate<T>(phase.get(), 0.25 + (0.25 * phasecv.get()))));
            tri.set(phase2tri(phase.get()));
            saw.set(phase2saw(phase.get()));
            square.set(phase2square(phase.get()));
            tri90.set(phase2tri(accumulate<T>(phase.get(), 0.25 + (0.25 * phasecv.get()))));
            phase90.set(accumulate<T>(phase.get(), 0.25 + (0.25 * phasecv.get())));
            qtri90.set(phase2qtri(accumulate<T>(phase.get(), 0.25 + (0.25 * phasecv.get()))));
            qtri.set(phase2qtri(phase.get()));
            randwave.set(interpolate<T>(interpolate<T>(randsrc.get(), randdst.get(), phase.get()), interpolate<T>(randsrc.get(), randdst.get(), (2.0 * phase.get()) - (phase.get() * phase.get())), randcv.get()));
            randwave2.set(interpolate<T>(interpolate<T>(randsrc2.get(), randdst2.get(), phase.get()), interpolate<T>(randsrc2.get(), randdst2.get(), (2.0 * phase.get()) - (phase.get() * phase.get())), randcv.get()));
        }

        void tick()
        {
            if(enable.get())
            {
                const T increment = frequency2period(clockFrequency.get()) / frequency2period(frequency.get());
                int reps = phasereps<T>(phase.get(), increment);

                while(reps != 0)
                {
                    randsrc.set(randdst.get());
                    randdst.set(random<T>(0, 1));
                    randsrc2.set(randdst2.get());
                    randdst2.set(random<T>(0, 1));
                    reps--;
                }

                const T value = accumulate<T>(phase.get(), increment);
                phase.set(value);
            }
            else
            {
                phase.set(cv.get());
            }
        }

        void reset()
        {
            phase.set(0);
        }
    };
}
