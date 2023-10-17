#include "../include/MagicNumbers.h"

bitboard movgen::get_bishops_attacks(bitboard blocker, bpos square, movgen::GeneratedMagics* gen_m)
{
    uint64_t index = blocker & gen_m->bishop_magics[square].mask;
    index *= gen_m->bishop_magics[square].magic;
    index >>= 64 - 9;
    return gen_m->bishop_attacks[square][index];
}

bitboard movgen::get_rook_attacks(bitboard blocker, bpos square, movgen::GeneratedMagics* gen_m)
{
    uint64_t index = blocker & gen_m->rook_magics[square].mask;
    index *= gen_m->rook_magics[square].magic;
    index >>= 64 - 12;
    return gen_m->rook_attacks[square][index];
}

uint64_t movgen::get_magic_index_bishop(bitboard blocker, Magic& magic)
{
    uint64_t index = blocker & magic.mask;
    index *= magic.magic;
    index >>= 64 - 9;
    return index;
}

uint64_t movgen::get_magic_index_rook(bitboard blocker, Magic& magic)
{
    uint64_t index = blocker & magic.mask;
    index *= magic.magic;
    index >>= 64 - 12;
    return index;
}

uint64_t movgen::_random_uint64()
{
    static std::random_device rd;
    std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<unsigned long long> dis(
        std::numeric_limits<std::uint64_t>::min(),
        std::numeric_limits<std::uint64_t>::max()
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

int movgen::count_1s(bitboard b)
{
    int r = 0;
    for (r = 0; b; r++, b &= b - 1);
    return r;
}

bitboard movgen::_generate_bishop_mask(bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8 - 1, x = sq % 8 - 1; (x > 0) && (y > 0); --x, --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8 - 1, x = sq % 8 + 1; (x < 7) && (y > 0); ++x, --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8 + 1, x = sq % 8 - 1; (x > 0) && (y < 7); --x, ++y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8 + 1, x = sq % 8 + 1; (x < 7) && (y < 7); ++x, ++y)
        movgen::set_bit(&attacks, y * 8 + x);

    return attacks;
}

bitboard movgen::_generate_bishop_moves(bitboard blocker, bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8 - 1, x = sq % 8 - 1; (x >= 0) && (y >= 0); --x, --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8 - 1, x = sq % 8 + 1; (x < 8) && (y >= 0); ++x, --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8 + 1, x = sq % 8 - 1; (x >= 0) && (y < 8); --x, ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8 + 1, x = sq % 8 + 1; (x < 8) && (y < 8); ++x, ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }

    return attacks;
}

bitboard movgen::_generate_rook_mask(bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8, x = sq % 8 - 1; x > 0; --x)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8 - 1, x = sq % 8; y > 0; --y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8 + 1, x = sq % 8; y < 7; ++y)
        movgen::set_bit(&attacks, y * 8 + x);
    for (int y = sq / 8, x = sq % 8 + 1; x < 7; ++x)
        movgen::set_bit(&attacks, y * 8 + x);

    return attacks;
}

bitboard movgen::_generate_rook_moves(bitboard blocker, bpos sq)
{
    bitboard attacks = 0;

    for (int y = sq / 8, x = sq % 8 - 1; x >= 0; --x)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8 - 1, x = sq % 8; y >= 0; --y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8 + 1, x = sq % 8; y < 8; ++y)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }
    for (int y = sq / 8, x = sq % 8 + 1; x < 8; ++x)
    {
        movgen::set_bit(&attacks, y * 8 + x);
        if (movgen::read_bit(blocker, y * 8 + x))
            break;
    }

    return attacks;
}

void movgen::generate_magics(movgen::GeneratedMagics* gen_m)
{
    for (bpos pos = 0; pos < 64; pos++)
    {
        Magic rook_magic, bishop_magic;

        // Exclude border bits from mask, because they are always blocking
        rook_magic.mask = _generate_rook_mask(pos);
        bishop_magic.mask = _generate_bishop_mask(pos);

        // Keep generating new magics until there is no collisions
        while (true)
        {
            bishop_magic.magic = _random_uint64_fewbits();
            if (count_1s((bishop_magic.mask * bishop_magic.magic) & 0xFF00000000000000ull) < 6)
                continue;
            if (try_fill_table_bishop(pos, bishop_magic, gen_m))
            {
                gen_m->bishop_magics[pos] = bishop_magic;
                break;
            }
        }
        while (true)
        {
            rook_magic.magic = _random_uint64_fewbits();
            if (count_1s((rook_magic.mask * rook_magic.magic) & 0xFF00000000000000ull) < 6)
                continue;
            if (try_fill_table_rook(pos, rook_magic, gen_m))
            {
                gen_m->rook_magics[pos] = rook_magic;
                break;
            }
        }
    }
}

bool movgen::try_fill_table_bishop(bpos sq, Magic& magic, movgen::GeneratedMagics* gen_m)
{
    bitboard blocker = 0;
    for (int i = 0; i < 512; i++) gen_m->bishop_attacks[sq][i] = 0;
    do
    {
        bitboard attacks = movgen::_generate_bishop_moves(blocker, sq);
        uint64_t index = movgen::get_magic_index_bishop(blocker, magic);

        if (gen_m->bishop_attacks[sq][index] == 0)
        {
            gen_m->bishop_attacks[sq][index] = attacks;
        }
        else if (gen_m->bishop_attacks[sq][index] != attacks)
        {
            return 0;
        }

        blocker = movgen::_get_next_blocker(blocker, magic.mask);
    } while (blocker != 0);

    return 1;
}

bool movgen::try_fill_table_rook(bpos sq, Magic& magic, movgen::GeneratedMagics* gen_m)
{
    bitboard blocker = 0;
    for (int i = 0; i < 4096; i++) gen_m->rook_attacks[sq][i] = 0;
    do
    {
        bitboard attacks = movgen::_generate_rook_moves(blocker, sq);
        uint64_t index = movgen::get_magic_index_rook(blocker, magic);

        if (gen_m->rook_attacks[sq][index] == 0)
        {
            gen_m->rook_attacks[sq][index] = attacks;
        }
        else if (gen_m->rook_attacks[sq][index] != attacks)
        {
            return 0;
        }
        blocker = movgen::_get_next_blocker(blocker, magic.mask);
    } while (blocker != 0);

    return 1;
}