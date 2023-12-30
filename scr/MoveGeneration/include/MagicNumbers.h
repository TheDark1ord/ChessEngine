#ifndef MAGIC_NUMBERS_H
#define MAGIC_NUMBERS_H

#include "MovgenTypes.h"

namespace movgen {
struct Magic {
  bitboard mask;
  uint64_t magic;
};

struct GeneratedMagics {
  bitboard bishop_attacks[64][512] = {0};
  bitboard rook_attacks[64][4096] = {0};

  Magic bishop_magics[64];
  Magic rook_magics[64];
};

/// @brief returns relevant moveset from previously generated moves
/// @param blocker positions of all the pieces on the board
/// @param square position of the bishop
/// @return gen_m - bishop moves
bitboard get_bishop_attacks(bitboard blocker, bpos square,
                            GeneratedMagics *gen_m);
/// @brief see movgen::get_bishop_attacks
bitboard get_rook_attacks(bitboard blocker, bpos square,
                          GeneratedMagics *gen_m);

// Get index in attacks array for given arguments
uint64_t get_magic_index_bishop(bitboard blocker, Magic &magic);
uint64_t get_magic_index_rook(bitboard blocker, Magic &magic);
///

void generate_magics(GeneratedMagics *gen_m);
/// @brief Fill the moves table for a given magic and search for a hash
/// collisions to check if a magic is valid
/// @return returns 1 if magic is valid, 0 if there is hash collisions
bool try_fill_table_bishop(bpos sq, Magic &magic, GeneratedMagics *gen_m);
/// @brief Fill the moves table for a given magic and search for a hash
/// collisions to check if a magic is valid
/// @return returns 1 if magic is valid, 0 if there is hash collisions
bool try_fill_table_rook(bpos sq, Magic &magic, GeneratedMagics *gen_m);

/// Helper funtions
uint64_t _random_uint64();
inline bitboard _random_uint64_fewbits();

inline bitboard _get_next_blocker(bitboard current, bitboard blocker_mask);
// This function is used to get all possible blocker board for a given
// blocker_mask
int count_1s(bitboard bitboard);

/// @brief generates mask for magic numbers(does not inlude border bits)
bitboard _generate_bishop_mask(bpos sq);
/// @brief Generates bishop moves for a given square,
/// unlike get_bishop_attacks, it generates them from scratch
/// @param sq square, bishop position
/// @param blocker positions of all the pieces on the board
/// @return bishop moves bitboard(includes blocked squares)
bitboard _generate_bishop_moves(bitboard blocker, bpos sq);

/// @brief see movgen::_generate_bishop_mask
bitboard _generate_rook_mask(bpos sq);
/// @brief see movgen::_generate_bishop_moves
bitboard _generate_rook_moves(bitboard blocker, bpos sq);
}; // namespace movgen

#endif
