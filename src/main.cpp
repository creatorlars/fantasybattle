#include <stdint.h>

struct Profile_Component
{
  double movement = 4.0;
  uint8_t strength = 3;
  uint8_t toughness = 3;
  uint8_t wounds = 1;
  uint8_t leadership = 1;
};

enum class Armour_Type : uint8_t
{
  light_armour,
  heavy_armour,
};

struct Armour_Component
{
  bool is_wielding = false;
  Armour_Type armour_type = Armour_Type::light_armour;
};

struct Shield_Component
{
  bool is_wielding = false;
};

uint8_t calculate_armour_save(const Armour_Component *armour, const Shield_Component *shield)
{
  uint8_t value = 7;
  if (armour != nullptr && shield == nullptr)
  {
    if (armour->armour_type == Armour_Type::light_armour)
    {
      value -= 1;
    }
    else if (armour->armour_type == Armour_Type::heavy_armour)
    {
      value -= 2;
    }
    if (shield != nullptr)
    {
      value -= 1;
    }
  }
  return value;
}

int main()
{

  // static application app;
  // app.run();
  return 0;
}