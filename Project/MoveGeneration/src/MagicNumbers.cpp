#include "../include/MagicNumbers.h"

bitboard movgen::get_bishops_attacks(bitboard pos, bpos square)
{
    int index = pos & movgen::bishop_magics[square].mask;
    index *= movgen::bishop_magics[square].magic;
    index >>= 64 - 9;
    return bishop_attacks[square][index];
}

bitboard movgen::get_rook_attacks(bitboard pos, bpos square)
{
    int index = pos & movgen::rook_magics[square].mask;
    index *= movgen::rook_magics[square].magic;
    index >>= 64 - 12;
    return rook_attacks[square][index];
}

uint64_t movgen::_random_uint64()
{
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max(),
        );
    return dis(gen);
}

bitboard movgen::_random_uint64_fewbits()
{
    return _random_uint64() & _random_uint64() & _random_uint64();
}

bitboard movgen::_get_next_blocker(bitboard current, bitboard blocker_mask)
{
    return (current - blocker_mask) & blocker_mask;
}

bitboard movgen::_generate_bishop_moves(bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8, x = sq % 8; (x >= 0) && (y >= 0); --x, --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; (x < 8) && (y >= 0); ++x, --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; (x >= 0) && (y < 8); --x, ++y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; (x < 8) && (y < 8); ++x, ++y)
        movgen::set_bit(&attacks, y * 8 + x);

    return attacks;
}

bitboard movgen::_generate_bishop_moves(bitboard blocker, bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8, x = sq % 8; (x >= 0) && (y >= 0); --x, --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; (x < 8) && (y >= 0); ++x, --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; (x >= 0) && (y < 8); --x, ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; (x < 8) && (y < 8); ++x, ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }

    return attacks;
}

bitboard movgen::_generate_rook_moves(bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8, x = sq % 8; x >= 0; --x)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; y >= 0; --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; y < 8; ++y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8; x < 8; ++x)
        movgen::set_bit(&attacks, y * 8 + x);

    return attacks;
}

bitboard movgen::_generate_rook_moves(bitboard blocker, bpos sq)
{
    for (int y = sq / 8, x = sq % 8; x >= 0; --x)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; y >= 0; --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; y < 8; ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8; x < 8; ++x)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (read_bit(attacks, y * 8 + x))
            break;
    }
}

void movgen::generate_magics()
{
}

void movgen::find_bishop_magic(bitboard blocker, bpos sq)
{
}

void movgen::find_rook_magic(bitboard blocker, bpos sq)
{
}