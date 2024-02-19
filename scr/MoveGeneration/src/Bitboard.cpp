#include "../include/Bitboard.h"
#include "../include/MagicNumbers.h"

bitboard Line[64][64];
bitboard Between[64][64];

void bitb::init()
{
    movgen::init_magics();

    for (int s1 = 0; s1 < 64; s1++)
    {
        for (int s2 = 0; s2 < 64; s2++)
        {
            if (movgen::get_rook_attacks(s1, 0) & sq_bitb(s2))
            {
                Line[s1][s2] = movgen::get_rook_attacks(s1, 0) & movgen::get_rook_attacks(s2, 0);
                Between[s1][s2] = movgen::get_rook_attacks(s1, sq_bitb(s2)) &
                    movgen::get_rook_attacks(s2, sq_bitb(s1)) | sq_bitb(s1) | sq_bitb(s2);
            }
            else if (movgen::get_bishop_attacks(s1, 0) & sq_bitb(s2))
            {
                Line[s1][s2] = movgen::get_bishop_attacks(s1, 0) & movgen::get_bishop_attacks(s2, 0);
                Between[s1][s2] = movgen::get_bishop_attacks(s1, sq_bitb(s2)) &
                    movgen::get_bishop_attacks(s2, sq_bitb(s1)) | sq_bitb(s1) | sq_bitb(s2);
            }
            else
            {
                Line[s1][s2] = 0;
                Between[s1][s2] = 0;
            }
        }
    }
}