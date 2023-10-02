#ifndef MAGIC_NUMBERS_H
#define MAGIC_NUMBERS_H

#include <random>
#include "Types.h"


namespace movgen
{
    struct Magic
    {
        bitboard mask;
        uint64_t magic;
    };

    bitboard bishop_attacks[64][512];
    bitboard rook_attacks[64][4096];

    Magic bishop_magics[64];
    Magic rook_magics[64];

    /// @brief returns relevant moveset from previously generated moves
    /// @param pos positions of all the pieces on the board
    /// @param square position of the bishop
    /// @return bishop moves
    bitboard get_bishops_attacks(bitboard pos, bpos square);
    /// @brief see movgen::get_bishops_attacks
    bitboard get_rook_attacks(bitboard pos, bpos square);

    ///Helper funtions

    uint64_t _random_uint64();
    inline bitboard _random_uint64_fewbits();
    // This function is used to get all possible blocker board for a given blocker_mask
    inline bitboard _get_next_blocker(bitboard current, bitboard blocker_mask);

    ///

    /// @brief Generates bishop moves for a given square,
    /// unlike get_bishops_attacks, it generates them from scratch
    /// @param sq square, bishop position
    /// @return bishop moves bitboard(includes blocked squares)
    bitboard _generate_bishop_moves(bpos sq);
    /// @brief Generates bishop moves for a given square,
    /// unlike get_bishops_attacks, it generates them from scratch
    /// @param sq square, bishop position
    /// @param blocker positions of all the pieces on the board
    /// @return bishop moves bitboard(includes blocked squares)
    bitboard _generate_bishop_moves(bitboard blocker, bpos sq);

    /// @brief see movgen::_generate_bishop_moves
    bitboard _generate_rook_moves(bpos sq);
    /// @brief see movgen::_generate_bishop_moves
    bitboard _generate_rook_moves(bitboard blocker, bpos sq);

    void generate_magics();
    void find_bishop_magic(bitboard blocker, bpos sq);
    void find_rook_magic(bitboard blocker, bpos sq);
};

#endif