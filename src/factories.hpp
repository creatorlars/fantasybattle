#pragma once
#include <entt/entt.hpp>
#include <components.hpp>

namespace fantasybattle
{

  inline entt::entity spawn(entt::registry &reg, const unit_profile &c1)
  {
    auto e = reg.create();
    reg.emplace<unit_profile>(e, c1);
    return e;
  }

  // inline entt::entity spawn(entt::registry &reg, const profile &c1, const armour &c2)
  // {
  //   auto e = reg.create();
  //   reg.emplace<profile>(e, c1);
  //   reg.emplace<armour>(e, c2);
  //   return e;
  // }

  // inline entt::entity spawn(entt::registry &reg, const profile &c1, const armour &c2, const shield &c3)
  // {
  //   auto e = reg.create();
  //   reg.emplace<profile>(e, c1);
  //   reg.emplace<armour>(e, c2);
  //   reg.emplace<shield>(e, c3);
  //   return e;
  // }

}
