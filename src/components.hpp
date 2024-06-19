#pragma once

#include <entt/entt.hpp>

namespace fantasybattle
{

  enum class armour_type
  {
    none,
    light,
    heavy,
    light_with_shield,
    heavy_with_shield
  };

  struct model
  {
    entt::entity unit;
    entt::entity player;
    bool is_character_model;
    int base_front_mm;
    int base_side_mm;
  };

  struct character_profile
  {
    int level;
  };

  struct unit_profile
  {
    double movement;
    int weapon_skill;
    int ballistic_skill;
    int strength;
    int toughness;
    int wounds;
    int initiative;
    int attacks;
    int leadership;
    int max_leadership_bonus;
    int intelligence;
    int cool;
    int willpower;
    armour_type armour;
  };

  struct unit
  {
    int ranks;
    int files;
    entt::entity player;
    std::vector<entt::entity> models;
  };

  struct player
  {
    std::vector<entt::entity> units;
    std::vector<entt::entity> characters;
  };

}
