#include <systems.hpp>
#include <factories.hpp>
#include <game_session.hpp>
#include <dice.hpp>

int main()
{
  fantasybattle::battlefield_system battlefield;
  fantasybattle::game_session game{battlefield};

  fantasybattle::unit_profile unit_profile_0{
      4.0,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      fantasybattle::armour_type::none};

  fantasybattle::unit_profile unit_profile_1{
      4.0,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      3,
      fantasybattle::armour_type::none};

  bool unit_dead_0 = false;
  bool unit_dead_1 = false;
  size_t combat_round = 0;

  while (unit_dead_0 == false && unit_dead_1 == false)
  {
    combat_round++;
    std::cout << "Combat Round " << combat_round << std::endl;

    if (unit_profile_0.initiative > unit_profile_1.initiative)
    {
      std::cout << "Unit 0 Acts First" << std::endl;
    }

    else if (unit_profile_0.initiative < unit_profile_1.initiative)
    {
      std::cout << "Unit 1 Acts First" << std::endl;
    }
    else
    {
      std::cout << "Units Act Simultaneously" << std::endl;
    }

    auto unit_0_to_hit_unit_1 = fantasybattle::minimum_to_hit_combat(unit_profile_0, unit_profile_1);
    auto unit_0_to_wound_unit_1 = fantasybattle::minimum_to_wound(unit_profile_0, unit_profile_1);

    if (unit_0_to_hit_unit_1)
    {
      std::cout << "Unit 0 needs to roll " << *unit_0_to_hit_unit_1 << " to hit Unit 1" << std::endl;
      int unit_0_roll_to_hit = fantasy_battle::roll_dice(1, 6);

      if (unit_0_roll_to_hit >= *unit_0_to_hit_unit_1)
      {
        std::cout << "Success! Unit 0 rolled a " << unit_0_roll_to_hit << std::endl;
        int unit_0_roll_to_wound = fantasy_battle::roll_dice(1, 6);

        std::cout << "Unit 0 needs to roll " << *unit_0_to_wound_unit_1 << " to wound Unit 1" << std::endl;

        if (unit_0_roll_to_wound >= *unit_0_to_wound_unit_1)
        {
          std::cout << "Success! Unit 0 rolled a " << unit_0_roll_to_wound << std::endl;
          unit_profile_1.wounds--;
        }
        else
        {

          std::cout << "Failure! Unit 0 rolled a " << unit_0_roll_to_wound << std::endl;
        }
      }
      else
      {
        std::cout << "Failure! Unit 1 rolled a " << unit_0_roll_to_hit << std::endl;
      }
    }
    else
    {
      std::cout << "Unit 0 cannot hit unit 1!" << std::endl;
    }

    if (unit_profile_0.wounds == 0)
    {
      unit_dead_0 = true;
      std::cout << "Unit 0 Has Died" << std::endl;
    }

    if (unit_profile_1.wounds == 0)
    {
      unit_dead_1 = true;
      std::cout << "Unit 1 Has Died" << std::endl;
    }
  }

  std::cout << "Battle Complete" << std::endl;

  // game.run();

  // static application app;
  // app.run();
  return 0;
}