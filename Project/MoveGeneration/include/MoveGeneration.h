#ifndef MOVGEN_H
#define MOVGEN_H

#include <vector>
#include <array>
#include <set>
#include <regex>
#include <exception>
#include <map>

#include "Types.h"
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

    // Calculated and cashed moves for faster computation
    struct MoveCashe
    {
        GeneratedMagics* magics;

        bitboard king_moves_bit[64];
        bitboard knight_moves_bit[64];
    };

    const char* fen_regex_string = "^"
        "(? <PiecePlacement>((? <RankItem>[pnbrqkPNBRQK1 - 8] {1, 8})\\ / ? ) { 8 })\\s +"
        "(? <SideToMove>b | w)\\s +"
        "(? <Castling>-| K ? Q ? k ? q)\\s +"
        "(? <EnPassant>-| [a - h][3 - 6])\\s +"
        "(? <HalfMoveClock>\\d + )\\s +"
        "(? <FullMoveNumber>\\d + )\\s *"
        "$";

    BoardPosition board_from_fen(std::string fen);
    std::string board_to_fen(BoardPosition& pos);

    // Second argument is the set for all positions reached within that game
    // Used to check for repetition; if null, check is not performed
    GeneratedMoves generateMoves(BoardPosition& pos, std::set<size_t>* hashed_pos = nullptr);

    // Slight optimisation: it is maybe possible to generate moves for a specific peace
    // and keep the rest intact
    // These functions generate pseudo legal moves for a given position
    // Generated moves is expected to be empty
    void generateKingMoves(BoardPosition& board, GeneratedMoves* moves, MoveCashe* cashe);
    void generateQueenMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateRookMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateBishopMoves(BoardPosition& board, GeneratedMoves* moves, GeneratedMagics* magics);
    void generateKnightMoves(BoardPosition& board, GeneratedMoves* moves, MoveCashe* cashe);
    void generatePawnMoves(BoardPosition& board, GeneratedMoves* moves);

    inline void generateQueenMoves(BoardPosition& board, GeneratedMoves* moves, MoveCashe* cashe)
    {
        movgen::generateQueenMoves(board, moves, cashe->magics);
    }
    inline void generateRookMoves(BoardPosition& board, GeneratedMoves* moves, MoveCashe* cashe)
    {
        movgen::generateRookMoves(board, moves, cashe->magics);
    }
    inline void generateBishopMoves(BoardPosition& board, GeneratedMoves* moves, MoveCashe* cashe)
    {
        movgen::generateBishopMoves(board, moves, cashe->magics);
    }

    void cacheKingMoves(MoveCashe* cashe);
    void cacheKnightMoves(MoveCashe* cashe);
}

#endif