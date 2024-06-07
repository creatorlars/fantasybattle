#include <systems.hpp>
#include <factories.hpp>
#include <game_session.hpp>

int main()
{
  fantasybattle::battlefield_system battlefield;
  fantasybattle::game_session game{battlefield};
  
  game.run();
  
  // static application app;
  // app.run();
  return 0;
}