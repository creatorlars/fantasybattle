#pragma once

#include <etl/bit.h>
#include <etl/byte.h>
#include <lzx/hardware/i2c.hpp>
#include <lzx/hardware/pin.hpp>

namespace lzx
{

    class i2c_device
    {
    public:
        i2c_device(i2c &driver, uint8_t baseAddr) : m_driver(driver), m_address(baseAddr) {}
        virtual ~i2c_device() {}

        void write_reg(uint8_t reg, uint8_t v)
        {
            m_driver.write_reg(m_address, reg, v);
        }

        uint8_t read_reg(uint8_t reg) const
        {
            return m_driver.read_reg(m_address, reg);
        }

         void write_bytes(uint8_t* data, size_t data_size)
         {
        	 m_driver.write_bytes(m_address, data, data_size);
         }

         void read_bytes(uint8_t*const data, size_t data_size)
         {

        	 m_driver.read_bytes(m_address, data, data_size);
         }
        uint8_t address() const
        {
            return m_address;
        }

        bool probe() const
        {
        	return m_driver.probe_addr(m_address);
        }

        template <uint8_t Addr, uint8_t Mask>
        class rw : public input_output_pin<uint8_t>
        {
        public:
            rw(i2c_device &regs) : m_regs(regs) {}
            virtual ~rw() {}

            uint8_t read() const override
            {
                return (m_regs.read_reg(Addr) >> shift) & max;
            }

            void write(uint8_t value) override
            {
                m_regs.write_reg(Addr, ((value & max) << shift) | (m_regs.read_reg(Addr) & ~Mask));
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint8_t shift = etl::countr_zero<uint8_t>(Mask);
            static constexpr uint8_t bits = etl::bit_width<uint8_t>(Mask);
            static constexpr uint8_t max = etl::max_value_for_nbits<bits>::value;

        private:
            i2c_device &m_regs;
        };

        template <uint8_t Addr, uint8_t Mask>
        class wos : public input_output_pin<uint8_t>
        {
        public:
            wos(i2c_device &regs, uint8_t initial = 0) : m_regs(regs), m_shadow(initial) {}
            virtual ~wos() {}

            uint8_t read() const override
            {
                return m_shadow;
            }

            void write(uint8_t value) override
            {
                m_regs.write_reg(Addr, ((value & max) << shift) | (m_shadow & ~Mask));
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint8_t shift = etl::countr_zero<uint8_t>(Mask);
            static constexpr uint8_t bits = etl::bit_width<uint8_t>(Mask);
            static constexpr uint8_t max = etl::max_value_for_nbits<bits>::value;

        private:
            i2c_device &m_regs;
            uint8_t m_shadow;
        };

        template <uint8_t Addr, uint8_t Mask>
        class wo : public output_pin<uint8_t>
        {
        public:
            wo(i2c_device &regs) : m_regs(regs) {}
            virtual ~wo() {}

            void write(uint8_t value) override
            {
                m_regs.write_reg(Addr, (value & max) << shift);
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint8_t shift = etl::countr_zero<uint8_t>(Mask);
            static constexpr uint8_t bits = etl::bit_width<uint8_t>(Mask);
            static constexpr uint8_t max = etl::max_value_for_nbits<bits>::value;

        private:
            i2c_device &m_regs;
        };

        template <uint8_t Addr, uint8_t Mask>
        class ro : public input_pin<uint8_t>
        {
        public:
            ro(i2c_device &regs) : m_regs(regs) {}
            virtual ~ro() {}

            uint8_t read() const override
            {
                return (m_regs.read_reg(Addr) & Mask) >> shift;
            }

            static constexpr uint8_t shift = etl::countr_zero<uint8_t>(Mask);
            static constexpr uint8_t bits = etl::bit_width<uint8_t>(Mask);
            static constexpr uint8_t max = etl::max_value_for_nbits<bits>::value;

        private:
            i2c_device &m_regs;
        };

        template <uint8_t AddrA, uint8_t MaskA, uint8_t AddrB, uint8_t MaskB>
        class rw2 : public input_output_pin<uint16_t>
        {
        public:
            rw2(i2c_device &regs) : m_regA(regs), m_regB(regs) {}
            virtual ~rw2() {}

            uint16_t read() const override
            {
                return (m_regA.read() << rw<AddrB, MaskB>::bits) | m_regB.read();
            }

            void write(const uint16_t value) override
            {
                m_regA.write(value >> rw<AddrB, MaskB>::bits);
                m_regB.write(value & m_regB.max);
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint16_t bits = rw<AddrA, MaskA>::bits + rw<AddrB, MaskB>::bits;
            static constexpr uint16_t max = etl::max_value_for_nbits<bits>::value;

        private:
            rw<AddrA, MaskA> m_regA;
            rw<AddrB, MaskB> m_regB;
        };

        template <uint8_t AddrA, uint8_t MaskA, uint8_t AddrB, uint8_t MaskB>
        class wo2 : public output_pin<uint16_t>
        {
        public:
            wo2(i2c_device &regs) : m_regA(regs), m_regB(regs) {}
            virtual ~wo2() {}

            void write(const uint16_t value) override
            {
                m_regA.write(value >> rw<AddrB, MaskB>::bits);
                m_regB.write(value & m_regB.max);
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint16_t bits = rw<AddrA, MaskA>::bits + rw<AddrB, MaskB>::bits;
            static constexpr uint16_t max = etl::max_value_for_nbits<bits>::value;

        private:
            wo<AddrA, MaskA> m_regA;
            wo<AddrB, MaskB> m_regB;
        };

        template <uint8_t AddrA, uint8_t MaskA, uint8_t AddrB, uint8_t MaskB>
        class ro2 : public input_pin<uint16_t>
        {
        public:
            ro2(i2c_device &regs) : m_regA(regs), m_regB(regs) {}
            virtual ~ro2() {}

            uint16_t read() const override
            {
                return (m_regA.read() << rw<AddrB, MaskB>::bits) | m_regB.read();
            }

            static constexpr uint16_t bits = rw<AddrA, MaskA>::bits + rw<AddrB, MaskB>::bits;
            static constexpr uint16_t max = etl::max_value_for_nbits<bits>::value;

        private:
            ro<AddrA, MaskA> m_regA;
            ro<AddrB, MaskB> m_regB;
        };

        template <uint8_t AddrA, uint8_t MaskA, uint8_t AddrB, uint8_t MaskB, uint8_t AddrC, uint8_t MaskC>
        class rw3 : public input_output_pin<uint32_t>
        {
        public:
            rw3(i2c_device &regs) : m_regA(regs), m_regB(regs), m_regC(regs) {}
            virtual ~rw3() {}

            uint32_t read() const override
            {
                return (m_regA.read() << rw<AddrB, MaskB>::bits << rw<AddrC, MaskC>::bits) | (m_regB.read() << rw<AddrC, MaskC>::bits) | m_regC.read();
            }

            void write(const uint32_t value) override
            {
                m_regA.write((value >> rw<AddrB, MaskB>::bits >> rw<AddrC, MaskC>::bits) & rw<AddrA, MaskA>::max);
                m_regB.write((value >> rw<AddrC, MaskC>::bits) & rw<AddrB, MaskB>::max);
                m_regC.write(value & rw<AddrC, MaskC>::max);
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint32_t bits = rw<AddrA, MaskA>::bits + rw<AddrB, MaskB>::bits + rw<AddrC, MaskC>::bits;
            static constexpr uint32_t max = etl::max_value_for_nbits<bits>::value;

        private:
            rw<AddrA, MaskA> m_regA;
            rw<AddrB, MaskB> m_regB;
            rw<AddrC, MaskC> m_regC;
        };

        template <uint8_t AddrA, uint8_t MaskA, uint8_t AddrB, uint8_t MaskB, uint8_t AddrC, uint8_t MaskC, uint8_t AddrD, uint8_t MaskD>
        class rw4 : public input_output_pin<uint32_t>
        {
        public:
            rw4(i2c_device &regs) : m_regA(regs), m_regB(regs), m_regC(regs), m_regD(regs) {}
            virtual ~rw4() {}

            uint32_t read() const override
            {
                return (m_regA.read() << rw<AddrB, MaskB>::bits << rw<AddrC, MaskC>::bits << rw<AddrD, MaskD>::bits) | (m_regB.read() << rw<AddrC, MaskC>::bits << rw<AddrD, MaskD>::bits) | (m_regC.read() << rw<AddrD, MaskD>::bits) | m_regD.read();
            }

            void write(const uint32_t value) override
            {
                m_regA.write((value >> rw<AddrB, MaskB>::bits >> rw<AddrC, MaskC>::bits >> m_regD.bits) & rw<AddrA, MaskA>::max);
                m_regB.write((value >> rw<AddrC, MaskC>::bits >> rw<AddrD, MaskD>::bits) & rw<AddrB, MaskB>::max);
                m_regC.write((value >> rw<AddrD, MaskD>::bits) & rw<AddrC, MaskC>::max);
                m_regD.write(value & rw<AddrD, MaskD>::max);
            }

            void set()
            {
                write(max);
            }

            void clear()
            {
                write(0);
            }

            static constexpr uint32_t bits = rw<AddrA, MaskA>::bits + rw<AddrB, MaskB>::bits + rw<AddrC, MaskC>::bits + rw<AddrD, MaskD>::bits;
            static constexpr uint32_t max = etl::max_value_for_nbits<bits>::value;

        private:
            rw<AddrA, MaskA> m_regA;
            rw<AddrB, MaskB> m_regB;
            rw<AddrC, MaskC> m_regC;
            rw<AddrD, MaskD> m_regD;
        };

    private:
        i2c &m_driver;
        uint8_t m_address;
    };

}
