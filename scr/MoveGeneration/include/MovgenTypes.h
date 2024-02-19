#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <vector>
#include "Bitboard.h"

namespace movgen {
    enum PieceType
    {
        NO_PIECE_TYPE,
        KING, QUEEN, ROOK, BISHOP, KNIGHT, PAWN,
        ANY,

        PIECE_TYPE_NB = 8,
    };

    enum Piece
    {
        NO_PIECE,
        B_KING = KING, B_QUEEN, B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN,
        W_KING = KING + 8, W_QUEEN, W_ROOK, W_BISHOP, W_KNIGHT, W_PAWN,

        WHITE_PIECES, BLACK_PIECES, ALL_PIECES = 0,
        PIECE_NB = 17
    };

    enum Color { WHITE, BLACK };

    enum CastlingRights
    {
        NO_CASTLING = 0,
        WHITE_SHORT = 0b0001,
        WHITE_LONG = 0b0010,
        BLACK_SHORT = 0b0100,
        BLACK_LONG = 0b1000,

        SHORT = WHITE_SHORT | BLACK_SHORT,
        LONG = WHITE_LONG | BLACK_LONG,
        WHITE_CASTLE = WHITE_SHORT | WHITE_LONG,
        BLACK_CASTLE = BLACK_SHORT | BLACK_LONG,
        ALL_CASTLE = WHITE_CASTLE | BLACK_CASTLE,

        CASTLING_NB
    };

    enum GenType
    {
        ATTACKS,
        QUIETS,
        ALL_MOVES
    };

    struct PositionInfo
    {
        bitboard pinned;
        bitboard pinners;

        bitboard checkers;
        // Squares to block check or capture the checker
        bitboard blockers;
        unsigned int check_num;
    };

    /*
    Bit numeration in bitboards:
    63 62 61 60 59 58 57 56
    ...
    7  6  5  4  3  2  1  0
    */
    struct BoardPosition {
        bitboard pieces[PIECE_NB];

        // Determines current side to move
        // 0 - White, 1 - Black
        Color side;
        CastlingRights castling;
        bpos en_passant;
        unsigned int halfmove;
        unsigned int fullmove;

        PositionInfo info;
    };

    struct Move {
        Piece piece;
        bpos from;
        bpos to;

        // Stores addition data about the move: piece captured(if any), promotion(if
        // any), etc... Data stored in order from LSb to MSb: Capture(4 bits):
        //      0 -- No capture
        //      1, 2 -- invalid
        //      3 to 12 in order: B_QUEEN, W_QUEEN, ROOK, BISHOP, KNIGHT, PAWN
        //      13 and 14 -- en passant capture (black and white respectively)
        //      15 -- unused
        // Promotion(4 bits):
        //      0 -- no promotion
        //      1 -- queen
        //      2 -- rook
        //      3 -- bishop
        //      4 -- knight
        //      5 -- any
        // Double pawn move(1 bit) -- used for faster detection to set the en passant
        // square Castling(2 bits) -- none, short, long
        //
        // Note: there is no validity check
        uint16_t move_data;

        /// @brief
        /// Constructs "Move" struct instance, 4 last params used to construct
        /// Move.move_data parameter
        /// @param piece
        /// @param from
        /// @param to
        /// @param capture see Move.move_data
        /// @param promotion see Move.move_data comment
        /// @param double_move if a pawn moved two squares forward
        /// @param castling see Move.move_data comment
        Move(Piece piece, bpos from, bpos to, unsigned char capture = 0,
            unsigned char promotion = 0, bool double_move = 0,
            unsigned char castling = 0);
    };

    /// @brief scans all of the bitboards on the given board and return the bitboard
    /// for which there is a bit set in a given position
    /// @param pos position to scan
    /// @param color if color of the piece is known, scan only bitboard of
    /// corresponding color
    ///     0 - scan all colors, 1 - scan only white, 2 - scan only black
    /// @return piece, which is in given board cell
    Piece get_piece(BoardPosition& b_pos, bpos pos, unsigned char color = 0);
    /// @brief
    /// @param b_pos board to get pointer to
    /// @param piece type of piece
    /// @return pointer to bitboard for specified piece type and color
    bitboard* get_bitboard_for_piece(BoardPosition& b_pos, Piece piece);

    class BoardHash {
        BoardHash(BoardPosition& pos);
        // Define a custom hash operator to use in std::set
        size_t operator()(BoardHash const& h) const;

        size_t hash;
        // Number of times this position was reached
        unsigned int reached = 1;
    };
}; // namespace movgen

// Define a hash function for BoardPosition
template <> struct std::hash<movgen::BoardPosition> {
    size_t operator()(movgen::BoardPosition const& p) const noexcept;
};

#endif
