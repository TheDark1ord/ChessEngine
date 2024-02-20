#include "../include/Bitboard.h"
#include "../include/MagicNumbers.h"

bitboard Line[64][64];
bitboard Between[64][64];
bitboard Between_in[64][64];

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
                Between_in[s1][s2] = movgen::get_rook_attacks(s1, sq_bitb(s2)) &
                    movgen::get_rook_attacks(s2, sq_bitb(s1));
                Between[s1][s2] = Between_in[s1][s2] | sq_bitb(s1) | sq_bitb(s2);
            }
            else if (movgen::get_bishop_attacks(s1, 0) & sq_bitb(s2))
            {
                Line[s1][s2] = movgen::get_bishop_attacks(s1, 0) & movgen::get_bishop_attacks(s2, 0);
                Between_in[s1][s2] = movgen::get_bishop_attacks(s1, sq_bitb(s2)) &
                    movgen::get_bishop_attacks(s2, sq_bitb(s1));
                Between[s1][s2] = Between_in[s1][s2] | sq_bitb(s1) | sq_bitb(s2);
            }
            else
            {
                Line[s1][s2] = 0;
                Between[s1][s2] = 0;
            }
        }
    }
}

std::vector<bpos> bitscan(bitboard board)
{
    // https://www.chessprogramming.org/BitScan#Bitscan_by_Modulo
    static constexpr int lookup67[68] = {
        64,  0,  1, 39,  2, 15, 40, 23,
        3, 12, 16, 59, 41, 19, 24, 54,
        4, -1, 13, 10, 17, 62, 60, 28,
        42, 30, 20, 51, 25, 44, 55, 47,
        5, 32, -1, 38, 14, 22, 11, 58,
        18, 53, 63,  9, 61, 27, 29, 50,
        43, 46, 31, 37, 21, 57, 52,  8,
        26, 49, 45, 36, 56,  7, 48, 35,
        6, 34, 33, -1
    };

    std::vector<bpos> set_bits;
    // TODO: Test optimal reserve number or even better update to better algorithm
    set_bits.reserve(8);

    while (board != 0)
    {
        set_bits.push_back(lookup67[(board & (~board + 1)) % 67]);
        board &= board - 1;
    }

    return set_bits;
}