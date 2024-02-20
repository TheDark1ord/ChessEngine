#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
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

    enum Color { BLACK, WHITE };

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

    enum MoveType
    {
        NORMAL,
        CAPTURE,
        PROMOTION,
        PROMOTION_CAPTURE,
        EN_PASSANT,
        DOUBLE_MOVE,
        CASTLING
    };

    enum GenType
    {
        ATTACKS,
        QUIETS,
        ALL_MOVES
    };

    struct PositionInfo
    {
        struct Pin
        {
            bpos pinned;
            bpos pinner;
            // Represents squares a pinned piece can move to
            bitboard mask;
        };
        std::vector<Pin> pins;
        bitboard pin_board = 0;
        // It is only possible to have one en_passant pin
        bool en_passant_pin = 0;

        bitboard checkers = 0;
        // Squares to block check or capture the checker
        bitboard blockers = 0;
        unsigned int checks_num = 0;

        //Squares attacked by the opposite side
        bitboard attacked;
    };

    /*
    Bit numeration in bitboards:
    63 62 61 60 59 58 57 56
    ...
    7  6  5  4  3  2  1  0
    */
    struct BoardPosition {
        bitboard pieces[PIECE_NB];
        Piece squares[64];

        // Determines current side to move
        // 0 - White, 1 - Black
        Color side_to_move;
        CastlingRights castling;
        bpos en_passant;
        unsigned int halfmove;
        unsigned int fullmove;

        PositionInfo info;
    };

    inline const char* fen_regex_string =
        "\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)"
        "\\s([b|w])\\s([K|Q|k|q]{1,4})\\s(-|[a-h][1-8])\\s(\\d+\\s\\d+)$";

    BoardPosition board_from_fen(std::string fen);
    std::string board_to_fen(BoardPosition& pos);

    struct Move {
        Piece piece;
        bpos from;
        bpos to;

        // Stores addition data about the move: piece captured(if any), promotion(if
        // any), etc... Data stored in order from LSb to MSb: Capture(4 bits):
        //      0 -- No capture
        //      1, 9 -- invalid
        //      2 to 6 in order: B_QUEEN , B_ROOK, B_BISHOP, B_KNIGHT, B_PAWN
        //      8 and 9 -- unused
        //      10 to 14 white pieces in the same order
        //      15 -- unused
        // Promotion(4 bits):
        //      0 -- no promotion
        //      1 -- invalid
        //      2 -- queen
        //      3 -- rook
        //      4 -- bishop
        //      5 -- knight
        //      6 -- any
        // Double pawn move(1 bit) -- used for faster detection to set the en passant
        // En passant (1 bit)
        // square Castling(2 bits) -- none, short, long
        //
        // Note: there is no validity check
        uint16_t move_data;

        Move(Piece piece, bpos from, bpos to);

        Move(Piece piece, bpos from, bpos to, unsigned char capture,
            unsigned char promotion = 0, bool double_move = 0, bool en_passant = 0,
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
