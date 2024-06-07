#pragma once
#include <components.hpp>
#include <entt/entt.hpp>
#include <optional>
#include <algorithm>

namespace fantasybattle
{

  inline std::optional<int> minimum_to_wound(const profile &attacker, const profile &defender)
  {
    const int result = std::max(2, 4 + defender.toughness - attacker.strength);

    if (result <= 6)
    {
      return result;
    }

    return {};
  }

  inline std::optional<int> minimum_to_hit_combat(const profile &attacker, const profile &defender)
  {
    const int result = std::max(2, 5 + ((defender.weapon_skill - attacker.weapon_skill) / 2));
    
    if (result <= 6)
    {
      return result;
    }

    return {};
  }

  inline std::optional<int> minimum_to_hit_shooting(const profile &attacker)
  {
    if (attacker.ballistic_skill >= 1)
    {
      return 7 - attacker.ballistic_skill;
    }

    return {};
  }

}
