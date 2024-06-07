#pragma once

#include <entt/entt.hpp>

#define DEFINE_TAG(x) typedef entt::tag<"x"_hs> x

namespace fantasybattle
{
  using namespace entt::literals;

  DEFINE_TAG(is_large_target);
  DEFINE_TAG(is_small_target);
  DEFINE_TAG(is_charging);
  DEFINE_TAG(is_routing);
  DEFINE_TAG(is_engaged_in_hand_to_hand_combat);
  DEFINE_TAG(is_on_difficult_ground);
  DEFINE_TAG(is_on_very_difficult_ground);
  DEFINE_TAG(is_within_four_inches_of_routing_units);
  DEFINE_TAG(is_charging);
  DEFINE_TAG(is_flying);
  DEFINE_TAG(is_flying_creature);
  DEFINE_TAG(is_following_up);
  DEFINE_TAG(is_frenzied);
  DEFINE_TAG(is_subject_to_animosity);
  DEFINE_TAG(is_wielding_two_weapons);
  DEFINE_TAG(is_divided_by_an_obstacle);
  DEFINE_TAG(is_unarmed);
  DEFINE_TAG(has_stomp_attack);
  DEFINE_TAG(has_gore_attack);
  DEFINE_TAG(has_bite_attack);
  DEFINE_TAG(has_claw_attack);
  DEFINE_TAG(has_tail_attack);
  DEFINE_TAG(failed_complex_manoeuvre_this_turn);

}

#undef DEFINE_TAG