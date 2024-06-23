#pragma once

#include <iostream>

namespace fantasy_battle
{
    static inline bool random_seed_initialized = false;

    inline int roll_dice(const int num_dice, const int sides_per_die, const int modifier = 0)
    {
        if(!random_seed_initialized)
        {
            srand(time(0));
            random_seed_initialized = true;
        }

        const int min = 1; // the min number a die can roll is 1
        const int max = sides_per_die;// this->dieSize; // the max value is the die size

        int result = 0;

        for(auto it = 0; it < num_dice; it++)
        {
            result += rand() % (max - min + 1) + min;
        }

        return result + modifier;
    }
}