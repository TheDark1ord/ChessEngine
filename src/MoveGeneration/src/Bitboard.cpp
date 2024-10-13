#include "../include/Bitboard.h"
#include "../include/MagicNumbers.h"
#include <cassert>

bitboard bitb::Line[64][64];
bitboard bitb::Between[64][64];
bitboard bitb::Between_in[64][64];
std::atomic_bool bitb::initialized = false;

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

    initialized = true;
}

std::vector<bpos> bitb::bitscan(bitboard board)
{
    std::vector<bpos> set_bits;
    // TODO: Test optimal reserve number or even better update to better algorithm
    set_bits.reserve(8);

    while (board)
    {
        set_bits.push_back(pop_lsb(board));
        board &= board - 1;
    }

    return set_bits;
}

bpos bitb::pop_lsb(bitboard board)
{
    assert(board);

#if defined(__GNUC__)  // GCC, Clang, ICX

    return bpos(__builtin_ctzll(board));

#elif defined(_MSC_VER)
#ifdef _WIN64  // MSVC, WIN64

    unsigned long idx;
    _BitScanForward64(&idx, board);
    return bpos(idx);

#else  // MSVC, WIN32
    unsigned long idx;

    if (b & 0xffffffff)
    {
        _BitScanForward(&idx, int32_t(board);
        return Square(idx);
    }
    else
    {
        _BitScanForward(&idx, int32_t(board >> 32));
        return bpos(idx + 32);
    }
#endif
#else  // Compiler is neither GCC nor MSVC compatible
#error "Compiler not supported."
#endif
}

unsigned char bitb::bit_count(bitboard board)
{
    int n = 0;
    while (board)
    {
        board &= board - 1;
        ++n;
    }
    return n;
}
