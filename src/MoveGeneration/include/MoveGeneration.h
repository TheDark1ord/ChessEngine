#ifndef MOVGEN_H
#define MOVGEN_H

#include <unordered_set>
#include <vector>

#include "Bitboard.h"
#include "MovgenTypes.h"

namespace movgen
{
    extern bitboard knight_attacks[64];
    extern bitboard king_attacks[64];

    void init();
    extern std::atomic_bool initialized;

    // Generates pseudo legal moves
    // Instanciated for every piece type except a king and a pawn
    template <movgen::PieceType type>
    void generate_piece_moves(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);

    template <>
    void generate_piece_moves<movgen::KING>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);

    template <movgen::Color color>
    void generate_pawn_moves(BoardPosition &pos, std::vector<Move> *generated);

    template <movgen::PieceType type>
    bitboard get_pseudo_attacks(bpos piece_pos, bitboard blocker);

    template <>
    bitboard get_pseudo_attacks<movgen::KING>(bpos piece_pos, bitboard blocker);
    template <>
    bitboard get_pseudo_attacks<movgen::QUEEN>(bpos piece_pos, bitboard blocker);
    template <>
    bitboard get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, bitboard blocker);
    template <>
    bitboard get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, bitboard blocker);
    template <>
    bitboard get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, bitboard blocker);

    // Finds the theckers and number of checks for current side and writes them to info
    template <Color them_c>
    void get_checkers(BoardPosition &pos, PositionInfo *info);
    // Finds pinned pieces and pinner pieces
    template <Color them_c>
    void get_pinners(BoardPosition &pos, PositionInfo *info);
    // Get squares, that are attacked by all pieces of that color
    template <Color color>
    void get_attacked(BoardPosition &pos, PositionInfo *info);

    template <movgen::Color color>
    std::vector<Move> *generate_all_moves(BoardPosition &pos);
    // Filters out non-legal moves from generated moves using PositionInfo
    std::vector<Move> *get_legal_moves(BoardPosition &pos, std::vector<Move> &generated);

    GameStatus make_move(movgen::BoardPosition *pos, movgen::Move &move, std::vector<movgen::Move> **new_moves);
    void undo_move(movgen::BoardPosition *pos, movgen::Move &move);
} // namespace movgen

/// Static functions for use in this file only
static bool is_legal(movgen::BoardPosition &pos, movgen::Move move);
// Converts bitboard to a move array adn pushes it to the std::vector<movgen::Move>* move_arr
static void bitb_movearray(movgen::Piece piece, bpos starting_pos, bitboard move_board,
                           bitboard them, movgen::BoardPosition &pos, std::vector<movgen::Move> *move_arr);

template <movgen::Color color, bitb::Direction d>
static void make_promotions(std::vector<movgen::Move> *move_arr, bpos to, unsigned char capture);

static void move_piece(movgen::BoardPosition *pos, movgen::Piece piece, bpos from, bpos to);

#endif
