#pragma once

#include <lzx/platform.hpp>
#include <lzx/hardware/pin.hpp>

namespace lzx
{

    template <typename T>
    class mem_regs
    {
    public:
        mem_regs(const uintptr_t baseAddr) : m_baseAddr(baseAddr) {}
        virtual ~mem_regs() {}

        void write(const uintptr_t a, const T v)
        {
            volatile T *addr = (volatile T *)(a + m_baseAddr);
            *addr = v;
        }

        T read(const uintptr_t a) const
        {
            return *(volatile T *)(a + m_baseAddr);
        }

        uintptr_t get_base_address() const
        {
            return m_baseAddr;
        }

        template <T Offset, T Shift, T Mask>
        class rw : public input_output_pin<T>
        {
        public:
            rw(const uintptr_t baseAddr) : m_baseAddr(baseAddr + Offset) {}
            virtual ~rw() {}

            T read() const override
            {
                T sample = *(volatile T *)m_baseAddr;
                sample = sample & Mask;
                sample = sample >> Shift;
                return sample;
            }

            void write(const T value) override
            {
                T sample = value;
                sample = sample << Shift;
                sample = sample & Mask;
                T existing = *(volatile T *)m_baseAddr;
                sample = sample | (existing & ~Mask);
                volatile T *addr = (volatile T *)m_baseAddr;
                *addr = sample;
            }

            void set()
            {
                write(Mask >> Shift);
            }

            void clear()
            {
                write(0);
            }

        private:
            const uintptr_t m_baseAddr;
        };

        template <T Offset, T Shift, T Mask>
        class wos : public input_output_pin<T>
        {
        public:
            wos(const uintptr_t baseAddr) : m_baseAddr(baseAddr + Offset) {}
            virtual ~wos() {}

            T read() const override
            {
                T sample = *(volatile T *)m_baseAddr;
                sample = sample & Mask;
                sample = sample >> Shift;
                return sample;
            }

            void write(const T value) override
            {
                T sample = value;
                sample = sample << Shift;
                sample = sample & Mask;
                T existing = *(volatile T *)m_baseAddr;
                sample = sample | (existing & ~Mask);
                volatile T *addr = (volatile T *)m_baseAddr;
                *addr = sample;
            }

            void set()
            {
                write(Mask >> Shift);
            }

            void clear()
            {
                write(0);
            }

        private:
            const uintptr_t m_baseAddr;
        };

        template <T Offset, T Shift, T Mask>
        class wo : public output_pin<T>
        {
        public:
            wo(const uintptr_t baseAddr) : m_baseAddr(baseAddr + Offset) {}
            virtual ~wo() {}

            void write(const T value) override
            {
                T sample = value;
                sample = sample << Shift;
                sample = sample & Mask;
                volatile T *addr = (volatile T *)m_baseAddr;
                *addr = sample;
            }

            void set()
            {
                write(Mask >> Shift);
            }

            void clear()
            {
                write(0);
            }

        private:
            const uintptr_t m_baseAddr;
        };

        template <T Offset, T Shift, T Mask>
        class ro : public input_pin<T>
        {
        public:
            ro(const uintptr_t baseAddr) : m_baseAddr(baseAddr + Offset) {}
            virtual ~ro() {}

            T read() const override
            {
                T sample = *(volatile T *)m_baseAddr;
                sample = sample & Mask;
                sample = sample >> Shift;
                return sample;
            }

        private:
            const uintptr_t m_baseAddr;
        };

    private:
        const uintptr_t m_baseAddr;
    };

}
