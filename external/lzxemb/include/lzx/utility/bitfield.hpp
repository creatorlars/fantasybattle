#pragma once

#include <etl/binary.h>

namespace lzx
{

  template <typename ValueType, typename FieldType, uint32_t Mask>
  struct bitfield
  {
    typedef ValueType value_type;
    typedef FieldType field_type;
    static constexpr value_type mask_v = Mask;
    static constexpr value_type shift_v = etl::count_trailing_zeros<value_type>(value_type(Mask));
    static constexpr field_type read(value_type value)
    {
      return field_type((value & mask_v) >> shift_v);
    }
    static constexpr value_type write(value_type value, field_type field)
    {
      return (value & ~mask_v) | ((value_type(field) << shift_v) & mask_v);
    }
  };
}