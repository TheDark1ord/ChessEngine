#ifndef MOVGEN_H
#define MOVGEN_H

#include <exception>
#include <map>
#include <regex>
#include <set>
#include <vector>

#include "Bitboard.h"
#include "MagicNumbers.h"
#include "MovgenTypes.h"

namespace movgen {
    extern bitboard knight_attacks[64];
    extern bitboard king_attacks[64];

    void init_movgen();

    // Generates pseudo legal moves
    template<movgen::PieceType type>
    void generate_piece_moves(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);

    // Instantiate this function for every piece type(except a pawn)
    template void generate_piece_moves<movgen::KING>(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);
    template void generate_piece_moves<movgen::QUEEN>(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);
    template void generate_piece_moves<movgen::ROOK>(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);
    template void generate_piece_moves<movgen::BISHOP>(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);
    template void generate_piece_moves<movgen::KNIGHT>(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>& generated);

    template<movgen::Color c, movgen::GenType type>
    void generate_pawn_moves(BoardPosition& pos, std::vector<Move>& generated);

    template<movgen::PieceType type>
    bitboard get_pseudo_attacks(bpos piece_pos, bitboard blocker);

    template<> bitboard get_pseudo_attacks<movgen::KING>(bpos piece_pos, bitboard blocker);
    template<> bitboard get_pseudo_attacks<movgen::QUEEN>(bpos piece_pos, bitboard blocker);
    template<> bitboard get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, bitboard blocker);
    template<> bitboard get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, bitboard blocker);
    template<> bitboard get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, bitboard blocker);

    // Finds the theckers and number of checks for current side and writes them to info
    template <Color c>
    void get_checkers(BoardPosition pos, PositionInfo* info);
    // Finds pinned pieces and pinner pieces
    template <Color c>
    void get_pinners(BoardPosition pos, PositionInfo* info);
    // Get squares, that are attacked by all pieces of that color
    template <Color c>
    void get_attacked(BoardPosition pos, PositionInfo* info);

    template<movgen::Color c>
    std::vector<Move> generate_all_moves(BoardPosition& pos);
    // Filters out non-legal moves from generated moves using PositionInfo
    std::vector<Move> get_legal_moves(BoardPosition& pos, std::vector<Move>& generated);

} // namespace movgen

// Static functions for use in this file only
static bool is_legal(movgen::BoardPosition& pos, movgen::Move move);
static void bitb_movearray(movgen::Piece piece, bpos starting_pos, bitboard move_board,
    bitboard them, movgen::BoardPosition& pos, std::vector<movgen::Move>* move_arr);

static movgen::MoveType get_move_type(movgen::Move move);
static movgen::Color get_piece_color(movgen::Piece piece);
static constexpr movgen::PieceType get_piece_type(movgen::Piece piece);
static constexpr movgen::Piece get_piece_from_type(movgen::PieceType type, movgen::Color c);


template<movgen::Color c, bitb::Direction d>
static void make_promotions(std::vector<movgen::Move>* move_arr, bpos to, unsigned char capture);

#endif
