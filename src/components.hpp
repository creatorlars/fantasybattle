#pragma once

namespace fantasybattle
{
  struct profile
  {
    double movement{4.0};
    int weapon_skill{3};
    int ballistic_skill{3};
    int strength{3};
    int toughness{3};
    int wounds{1};
    int leadership{6};
  };

  enum class armour_type
  {
    light,
    heavy
  };

  struct armour
  {
    armour_type type{armour_type::light};
  };

  struct shield
  {
  };

  struct unit
  {
    
  };

}
