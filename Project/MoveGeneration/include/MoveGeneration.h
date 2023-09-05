#ifndef MOVGEN_H
#define MOVGEN_H

#include <vector>
#include <set>
#include <regex>
#include <exception>
#include <map>

typedef unsigned char bpos;
typedef unsigned long long bitboard;

enum Piece
{
    NONE = 0, // reserved for capture data
    B_KING, W_KING,
    B_QUEEN, W_QUEEN,
    B_ROOK, W_ROOK,
    B_BISHOP, W_BISHOP,
    B_KNIGHT, W_KNIGHT,
    B_PAWN, W_PAWN
};

namespace movgen
{
    struct Move
    {
        Piece piece;
        bpos from;
        bpos to;

        // Stores addition data about the move: piece captured(if any), promotion(if any), etc...
        // Data stored in order from LSb to MSb:
        // Capture(4 bits):
        //      0 - No capture
        //      1, 2 -- invalid
        //      3 to 12 in order: B_QUEEN, W_QUEEN, ROOK, BISHOP, KNIGHT, PAWN
        //      13 and 14 -- en passant capture (black and white respectively)
        //      15 -- unused
        // Promotion(4 bits):
        //      TODO:
        // Double pawn move(1 bit) -- used for faster detection to set the en passant square
        // Castling(2 bits) -- none, short, long
        //
        // Note: there is no validity check
        uint16_t move_data;
    };

    struct BoardPosition
    {
        bitboard b_kings = 0;
        bitboard w_kings = 0;
        bitboard b_queens = 0;
        bitboard w_queens = 0;
        bitboard b_rooks = 0;
        bitboard w_rooks = 0;
        bitboard b_bishops = 0;
        bitboard w_bishops = 0;
        bitboard b_knights = 0;
        bitboard w_knights = 0;
        bitboard b_pawns = 0;
        bitboard w_pawns = 0;

        bitboard b_pieces = 0;
        bitboard w_pieces = 0;
        bitboard all_pieces = 0;

        bool move;
        // From LSB to MSB:
        // White short, long; black short, long
        unsigned char castling;
        bpos en_passant;
        unsigned int halfmove;
        unsigned int fullmove;
    };

    inline void set_bit(bitboard* b, bpos pos);
    inline void cleat_bit(bitboard* b, bpos pos);
    inline void flip_bit(bitboard* b, bpos pos);
    inline bool read_bit(bitboard& b, bpos pos);

    class BoardHash
    {
        BoardHash(BoardPosition& pos);
        // Define a custom hash operator to use in std::set
        size_t operator()(BoardHash const& h) const;

        size_t hash;
        // Number of times this position was reached
        unsigned int reached = 1;
    };

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

        std::vector<Move> pos_moves;
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
    void generateKingMoves(BoardPosition& pos, GeneratedMoves* moves);
    void generateQueenMoves(BoardPosition& pos, GeneratedMoves* moves);
    void generateRookMoves(BoardPosition& pos, GeneratedMoves* moves);
    void generateBishopMoves(BoardPosition& pos, GeneratedMoves* moves);
    void generateKnightMoves(BoardPosition& pos, GeneratedMoves* moves);
    void generatePawnMoves(BoardPosition& pos, GeneratedMoves* moves);
}

// Define a hash function for BoardPosition
template<>
struct std::hash<movgen::BoardPosition>
{
    size_t operator()(movgen::BoardPosition const& p) const noexcept;
};

#endif