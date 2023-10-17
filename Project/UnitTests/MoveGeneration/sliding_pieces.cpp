#include <iostream>
#include "MoveGeneration.h"
#include "MagicNumbers.h"
#include "Types.h"

int main(int argc, char* argv[])
{
    movgen::GeneratedMagics* magics = new movgen::GeneratedMagics;

    movgen::generate_magics(magics);

    if (argc == 1)
    {
        for (int i = 0; i < 10; i++)
        {
            bitboard blocker_mask = movgen::_random_uint64();
            bpos bishop_pos = static_cast<bpos>(movgen::_random_uint64() % 64);

            bitboard expected = movgen::_generate_bishop_moves(blocker_mask, bishop_pos);
            if (expected != movgen::get_bishops_attacks(blocker_mask, bishop_pos, magics))
                return -1;
        }

        for (int i = 0; i < 10; i++)
        {
            bitboard blocker_mask = movgen::_random_uint64();
            bpos rook_pos = static_cast<bpos>(movgen::_random_uint64() % 64);

            bitboard expected = movgen::_generate_rook_moves(blocker_mask, rook_pos);
            if (expected != movgen::get_rook_attacks(blocker_mask, rook_pos, magics))
                return -1;
        }
    }
    else
    {
        //TODO: SLiding piece test arguments
    }

    return 0;
}