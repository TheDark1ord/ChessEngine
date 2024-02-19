#ifndef BITBOARD_H
#define BITBOARD_H

#include <vector>

/*
    Bit numeration:
    63 62 61 60 59 58 57 56
    ...
    7  6  5  4  3  2  1  0
*/
typedef unsigned long long bitboard;
typedef unsigned char bpos;

namespace bitb {
    // Initializes bitboard global variables
    void init();

    constexpr void set_bit(bitboard* b, bpos pos) { *b |= 1ull << pos; }
    constexpr void clear_bit(bitboard* b, bpos pos) { *b |= ~(1ull << pos); }
    constexpr void flip_bit(bitboard* b, bpos pos) { *b ^= 1ull << pos; }
    constexpr bool read_bit(bitboard& b, bpos pos) { return b & (1ull << pos); }

    enum Direction
    {
        LEFT = 1, RIGHT = -1,
        UP = 8, DOWN = -8
    };
};

// Converts a square position to bitboard with 1 in that position
constexpr bitboard sq_bitb(bpos pos) { return 1ull << pos; }

/// @brief converts a bitboard into a sequense of values between 0 and 63, that
/// correspond to positions of set bits in the bitboard
inline std::vector<bpos> bitscan(bitboard board);
inline bpos pop_lsb(bitboard* board);
inline unsigned char nonzero_count(bitboard board);

constexpr bitboard Rank[8]{
    0x101010101010101ull,
    0x101010101010101ull << 1,
    0x101010101010101ull << 2,
    0x101010101010101ull << 3,
    0x101010101010101ull << 4,
    0x101010101010101ull << 5,
    0x101010101010101ull << 6,
    0x101010101010101ull << 7
};
constexpr bitboard File[8]{
    0xFFull,
    0xFFull << (8 * 1),
    0xFFull << (8 * 2),
    0xFFull << (8 * 3),
    0xFFull << (8 * 4),
    0xFFull << (8 * 5),
    0xFFull << (8 * 6),
    0xFFull << (8 * 7),
};

// Common line(from edge to edge) between two squares (if exists)
extern bitboard Line[64][64];
// A line connecting two squares(includes these squares) (if exists)
extern bitboard Between[64][64];

constexpr bitboard rank_bitb(bpos sq) { return Rank[sq >> 3]; }
constexpr bitboard file_bitb(bpos sq) { return File[sq & 0b111]; }

template<bitb::Direction D>
constexpr bitboard shift(bitboard bitb)
{
    return D == bitb::UP ? bitb << 8
        : D == bitb::UP + bitb::UP ? bitb << 16
        : D == bitb::UP + bitb::LEFT ? (bitb & ~File[7]) << 9
        : D == bitb::UP + bitb::RIGHT ? (bitb & ~File[0]) << 7
        : D == bitb::DOWN ? bitb >> 8
        : D == bitb::DOWN + bitb::DOWN ? bitb >> 16
        : D == bitb::DOWN + bitb::LEFT ? (bitb & ~File[7]) >> 7
        : D == bitb::DOWN + bitb::RIGHT ? (bitb & ~File[0]) >> 9
        : D == bitb::LEFT ? (bitb & ~File[7]) << 1
        : D == bitb::RIGHT ? (bitb & ~File[0]) >> 1
        : 0;
};

#endif //BITBOARD_H