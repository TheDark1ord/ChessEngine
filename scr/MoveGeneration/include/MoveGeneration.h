#ifndef MOVGEN_H
#define MOVGEN_H

#include <vector>
#include <set>
#include <regex>
#include <exception>
#include <map>

#include "MovgenTypes.h"
#include "MagicNumbers.h"

namespace movgen
{
    struct GeneratedMoves
    {
        bitboard b_king_attacks = 0;
        bitboard w_king_attacks = 0;
        bitboard b_queen_attacks = 0;
        bitboard w_queen_attacks = 0;
        bitboard b_rook_attacks = 0;
        bitboard w_rook_attacks = 0;
        bitboard b_bishop_attacks = 0;
        bitboard w_bishop_attacks = 0;
        bitboard b_knight_attacks = 0;
        bitboard w_knight_attacks = 0;
        bitboard b_pawn_attacks = 0;
        bitboard w_pawn_attacks = 0;

        // All squares that are under attack of respective side
        bitboard b_attacks = 0;
        bitboard w_attacks = 0;

        std::vector<Move> moves;
    };

    /// @brief Contains all of the cach, generated or loaded from a file
    /// at the start of the programm, that is used as a lookup for move generation
    struct MoveCache
    {
        GeneratedMagics* magics;

        // Should be very easy to generate(it is)
        bitboard knight_cache[64];
        bitboard king_cache[64];
    };

    void generate_move_cache(MoveCache* out);

    // inline const char* fen_regex_string = "^"
    //     "(? <PiecePlacement>((? <RankItem>[pnbrqkPNBRQK1 - 8] {1, 8})\\ / ? ) { 8 })\\s +"
    //     "(? <SideToMove>b | w)\\s +"
    //     "(? <Castling>-| K ? Q ? k ? q)\\s +"
    //     "(? <EnPassant>-| [a - h][3 - 6])\\s +"
    //     "(? <HalfMoveClock>\\d + )\\s +"
    //     "(? <FullMoveNumber>\\d + )\\s *"
    //     "$";

    inline const char* fen_regex_string = "\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)"
        "\\s([b|w])\\s([K|Q|k|q]{1,4})\\s(-|[a-h][1-8])\\s(\\d+\\s\\d+)$";

    BoardPosition board_from_fen(std::string fen);
    std::string board_to_fen(BoardPosition& pos);

    /// This is good, but why do I need this?
    /// TODO: do not forget to add this to make_move function
    /// ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
    /// ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓ ↓
    // Second argument is the set for all positions reached within that game
    // Used to check for repetition; if null, check is not performed
    GeneratedMoves generateMoves(BoardPosition& pos, std::set<size_t>* hashed_pos = nullptr);

    // Slight optimisation: it is maybe possible to generate moves for a specific peace
    // and keep the rest intact
    // These functions generate pseudo legal moves for a given position
    // Generated moves is expected to be empty
    void generateKingMoves(BoardPosition& board, GeneratedMoves* moves, MoveCache* cache);
    void generateQueenMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateRookMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateBishopMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateKnightMoves(BoardPosition& board, GeneratedMoves* moves, MoveCache* cache);
    void generatePawnMoves(BoardPosition& board, GeneratedMoves* moves);
}

#endif