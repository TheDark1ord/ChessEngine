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

    inline const char* fen_regex_string =
        "\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)"
        "\\s([b|w])\\s([K|Q|k|q]{1,4})\\s(-|[a-h][1-8])\\s(\\d+\\s\\d+)$";

    BoardPosition board_from_fen(std::string fen);
    std::string board_to_fen(BoardPosition& pos);

    // Generates pseudo legal moves
    template<movgen::PieceType type>
    void generate_piece_moves(bpos piece_pos, std::vector<Move>& generated, BoardPosition& pos);
    // Instantiate this function for every piece type(except a pawn)
    template<> void generate_piece_moves<movgen::QUEEN>(bpos piece_pos, std::vector<Move>& generated, BoardPosition& pos);
    template<> void generate_piece_moves<movgen::ROOK>(bpos piece_pos, std::vector<Move>& generated, BoardPosition& pos);
    template<> void generate_piece_moves<movgen::BISHOP>(bpos piece_pos, std::vector<Move>& generated, BoardPosition& pos);
    template<> void generate_piece_moves<movgen::KNIGHT>(bpos piece_pos, std::vector<Move>& generated, BoardPosition& pos);

    template<movgen::Color color, movgen::GenType type>
    bitboard generate_pawn_moves();

    template<movgen::PieceType type>
    bitboard get_pseudo_attacks(bpos piece_pos, BoardPosition pos);

    template<> bitboard get_pseudo_attacks<movgen::KING>(bpos piece_pos, BoardPosition pos);
    template<> bitboard get_pseudo_attacks<movgen::QUEEN>(bpos piece_pos, BoardPosition pos);
    template<> bitboard get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, BoardPosition pos);
    template<> bitboard get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, BoardPosition pos);
    template<> bitboard get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, BoardPosition pos);

    // Finds the theckers and number of checks for current side and writes them to info
    void get_checkers(BoardPosition pos, PositionInfo* info);
    // Finds pinned pieces and pinner pieces
    void get_pinners(BoardPosition pos, PositionInfo* info);

    // Filters out non-legal moves from generated moves using PositionInfo
    std::vector<Move> get_legal_moves(BoardPosition pos, PositionInfo& info, std::vector<Move>& generated);

    /// This is good, but why do I need this?
    /// TODO: do not forget to add this to make_move function
    /// ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
    /// ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
    // Second argument is the set for all positions reached within that game
    // Used to check for repetition; if null, check is not performed
    movgen::GeneratedMoves generate_moves(BoardPosition pos,
        std::set<size_t>* hashed_pos = nullptr);

    // Slight optimisation: it is maybe possible to generate moves for a specific
    // peace and keep the rest intact These functions generate pseudo legal moves
    // for a given position Generated moves is expected to be empty
    void generateKingMoves(BoardPosition& board, GeneratedMoves* moves,
        MoveCache* cache);
    void generateQueenMoves(BoardPosition& board, GeneratedMoves* moves,
        GeneratedMagics* magics);
    void generateRookMoves(BoardPosition& board, GeneratedMoves* moves,
        GeneratedMagics* magics);
    void generateBishopMoves(BoardPosition& board, GeneratedMoves* moves,
        GeneratedMagics* magics);
    void generateKnightMoves(BoardPosition& board, GeneratedMoves* moves,
        MoveCache* cache);
    void generatePawnMoves(BoardPosition& board, GeneratedMoves* moves);
} // namespace movgen

// Static functions for use in this file only


#endif
