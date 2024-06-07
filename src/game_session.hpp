#pragma once

#include "battlefield.hpp"
#include <iostream>
#include <vector>
#include <memory>

namespace fantasybattle
{

    class game_phase
    {
    public:
        virtual ~game_phase() {}
        virtual void enter() = 0;
        virtual void exit() = 0;
    };

    class movement_phase : public game_phase
    {
    public:
        void enter() override
        {
            std::cout << "Movement Phase Entry" << std::endl;
        }

        void exit() override
        {
            std::cout << "Movement Phase Exit" << std::endl;
        }
    };

    class combat_phase : public game_phase
    {
    public:
        void enter() override
        {
            std::cout << "Combat Phase Entry" << std::endl;
        }

        void exit() override
        {
            std::cout << "Combat Phase Exit" << std::endl;
        }
    };

    class game_session
    {
    public:
        game_session(battlefield_system &battlefield)
            : m_battlefield(battlefield),
              m_max_turns(6),
              m_current_turn(0)
        {
            m_phases.push_back(std::shared_ptr<game_phase>(new movement_phase()));
            m_phases.push_back(std::shared_ptr<game_phase>(new combat_phase()));
        }

        void run()
        {
            m_current_turn = 0;

            while (m_current_turn < m_max_turns)
            {
                std::cout << "Turn #" << m_current_turn + 1 << " Start" << std::endl;
                
                for (auto& phase : m_phases)
                {
                    phase->enter();
                    phase->exit();
                }

                std::cout << "Turn #" << m_current_turn + 1 << " End" << std::endl;

                m_current_turn++;
            }
        }

    private:
        std::vector<std::shared_ptr<game_phase>> m_phases;
        battlefield_system m_battlefield;
        const int m_max_turns;
        int m_current_turn;
    };

}