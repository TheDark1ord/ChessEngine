#include "MoveGeneration.h"
#include "MagicNumbers.h"
#include "MovgenTypes.h"

#include <type_traits>
#include <cstdint>
#include <random>
#include <iostream>

int main(int argc, char* argv[])
{
    movgen::init_magics();
    std::random_device rd;
    std::mt19937 gen(rd());

    if (argc == 1)
    {
        for (int i = 0; i < 10; i++)
        {
            bitboard blocker_mask = gen;
            bpos bishop_pos = static_cast<bpos>(gen % 64);

            bitboard expected = movgen::_generate_bishop_moves(blocker_mask, bishop_pos);
            if (expected != movgen::get_bishop_attacks(blocker_mask, bishop_pos))
                return -1;
        }

        for (int i = 0; i < 10; i++)
        {
            bitboard blocker_mask = gen;
            bpos rook_pos = static_cast<bpos>(gen % 64);

            bitboard expected = movgen::_generate_rook_moves(blocker_mask, rook_pos);
            if (expected != movgen::get_rook_attacks(blocker_mask, rook_pos))
                return -1;
        }
    }
    else
    {
        //TODO: Sliding piece test arguments
    }

    return 0;
}