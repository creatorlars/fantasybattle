#pragma once

#include <lzx/utility/bitfield.hpp>
#include <etl/array.h>

namespace lzx
{

  template <typename ValueType, typename ComponentType, ValueType aMask, ValueType yMask, ValueType uMask, ValueType vMask, ValueType rMask, ValueType gMask, ValueType bMask>
  struct pixel_format
  {
    typedef ValueType value_type;
    typedef ComponentType component_type;
    typedef bitfield<value_type, component_type, aMask> a;
    typedef bitfield<value_type, component_type, yMask> y;
    typedef bitfield<value_type, component_type, uMask> u;
    typedef bitfield<value_type, component_type, vMask> v;
    typedef bitfield<value_type, component_type, rMask> r;
    typedef bitfield<value_type, component_type, gMask> g;
    typedef bitfield<value_type, component_type, bMask> b;
    static constexpr bool has_a_v = a::mask_v != 0;
    static constexpr bool has_y_v = y::mask_v != 0;
    static constexpr bool has_u_v = u::mask_v != 0;
    static constexpr bool has_v_v = v::mask_v != 0;
    static constexpr bool has_r_v = r::mask_v != 0;
    static constexpr bool has_g_v = g::mask_v != 0;
    static constexpr bool has_b_v = b::mask_v != 0;
    static constexpr bool num_a_bits_v = etl::count_bits<value_type>(a::mask_v);
    static constexpr bool num_y_bits_v = etl::count_bits<value_type>(y::mask_v);
    static constexpr bool num_u_bits_v = etl::count_bits<value_type>(u::mask_v);
    static constexpr bool num_v_bits_v = etl::count_bits<value_type>(v::mask_v);
    static constexpr bool num_r_bits_v = etl::count_bits<value_type>(r::mask_v);
    static constexpr bool num_g_bits_v = etl::count_bits<value_type>(g::mask_v);
    static constexpr bool num_b_bits_v = etl::count_bits<value_type>(b::mask_v);
    static constexpr value_type mask_v = a::mask_v | y::mask_v | u::mask_v | v::mask_v | r::mask_v | g::mask_v | b::mask_v;
    static constexpr size_t bits_per_pixel_v = sizeof(value_type) * 8;
    static constexpr size_t bits_per_pixel_unused_v = bits_per_pixel_v - etl::count_bits<value_type>(mask_v);
    static constexpr size_t num_components_v = size_t(has_a_v) + size_t(has_y_v) + size_t(has_u_v) + size_t(has_v_v) + size_t(has_r_v) + size_t(has_g_v) + size_t(has_b_v);

    template <typename T>
    static constexpr value_type from_argb(const etl::array<T, 4> &values)
    {
      static_assert(has_a_v);
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      value_type res = value_type(0);
      res = a::write(res, component_type(values[0]));
      res = r::write(res, component_type(values[1]));
      res = g::write(res, component_type(values[2]));
      res = b::write(res, component_type(values[3]));
      return res;
    }

    template <typename T>
    static constexpr etl::array<T, 4> to_argb(const value_type &value)
    {
      static_assert(has_a_v);
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      etl::array<T, 3> res;
      res[0] = T(a::read(value));
      res[1] = T(r::read(value));
      res[2] = T(g::read(value));
      res[3] = T(b::read(value));
      return res;
    }

    template <typename T>
    static constexpr value_type from_rgba(const etl::array<T, 4> &values)
    {
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      static_assert(has_a_v);
      value_type res = value_type(0);
      res = r::write(res, component_type(values[0]));
      res = g::write(res, component_type(values[1]));
      res = b::write(res, component_type(values[2]));
      res = a::write(res, component_type(values[3]));
      return res;
    }

    template <typename T>
    static constexpr etl::array<T, 4> to_rgba(const value_type &value)
    {
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      static_assert(has_a_v);
      etl::array<T, 3> res;
      res[0] = T(r::read(value));
      res[1] = T(g::read(value));
      res[2] = T(b::read(value));
      res[3] = T(a::read(value));
      return res;
    }

    template <typename T>
    static constexpr value_type from_rgb(const etl::array<T, 3> &values)
    {
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      value_type res = value_type(0);
      res = r::write(res, component_type(values[0]));
      res = g::write(res, component_type(values[1]));
      res = b::write(res, component_type(values[2]));
      return res;
    }

    template <typename T>
    static constexpr etl::array<T, 3> to_rgb(const value_type &value)
    {
      static_assert(has_r_v);
      static_assert(has_g_v);
      static_assert(has_b_v);
      etl::array<T, 3> res;
      res[0] = T(r::read(value));
      res[1] = T(g::read(value));
      res[2] = T(b::read(value));
      return res;
    }

    template <typename T>
    static constexpr value_type from_yuv(const etl::array<T, 3> &values)
    {
      static_assert(has_y_v);
      static_assert(has_u_v);
      static_assert(has_v_v);
      value_type res = value_type(0);
      res = y::write(res, component_type(values[0]));
      res = u::write(res, component_type(values[1]));
      res = v::write(res, component_type(values[2]));
      return res;
    }

    template <typename T>
    static constexpr etl::array<T, 3> to_yuv(const value_type &value)
    {
      static_assert(has_y_v);
      static_assert(has_u_v);
      static_assert(has_v_v);
      etl::array<T, 3> res;
      res[0] = T(y::read(value));
      res[1] = T(u::read(value));
      res[2] = T(v::read(value));
      return res;
    }

    template <typename T>
    static constexpr value_type from_y(const T &value)
    {
      static_assert(has_y_v);
      value_type res = value_type(0);
      res = y::write(res, component_type(value));
      return res;
    }

    template <typename T>
    static constexpr T to_y(const value_type &value)
    {
      static_assert(has_y_v);
      T res;
      res = T(y::read(value));
      return res;
    }
  };

  typedef pixel_format<uint32_t, uint8_t, 255 << 24, 0, 0, 0, 255, 255 << 8, 255 << 16> abgr32;
  typedef pixel_format<uint32_t, uint8_t, 255 << 24, 0, 0, 0, 255 << 16, 255 << 8, 255> argb32;
  typedef pixel_format<uint32_t, uint8_t, 255, 0, 0, 0, 255 << 24, 255 << 16, 255 << 8> rgba32;
  typedef pixel_format<uint32_t, uint8_t, 0, 0, 0, 0, 255 << 16, 255 << 8, 255> rgb24;
  typedef pixel_format<uint16_t, uint8_t, 0, 0, 0, 0, 0b11111 << 11, 0b111111 << 5, 0b11111> rgb16;

}