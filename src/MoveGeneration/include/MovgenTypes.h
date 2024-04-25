#ifndef TYPES_H
#define TYPES_H

#include <cstdint>
#include <string>
#include <vector>
#include "Bitboard.h"

namespace movgen
{
    enum PieceType
    {
        NO_PIECE_TYPE,
        KING,
        QUEEN,
        ROOK,
        BISHOP,
        KNIGHT,
        PAWN,
        ANY,

        PIECE_TYPE_NB = 8,
    };

    enum Piece
    {
        NO_PIECE,
        B_KING = KING,
        B_QUEEN,
        B_ROOK,
        B_BISHOP,
        B_KNIGHT,
        B_PAWN,
        W_KING = KING + 8,
        W_QUEEN,
        W_ROOK,
        W_BISHOP,
        W_KNIGHT,
        W_PAWN,

        BLACK_PIECES = 7,
        WHITE_PIECES = BLACK_PIECES + 8,
        ALL_PIECES = 0,
        PIECE_NB = 17
    };

    enum Color
    {
        BLACK,
        WHITE
    };

    enum GameStatus
    {
        GAME_CONTINUES,
        DRAW,
        WHITE_WINS,
        BLACK_WINS
    };

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
        REGULAR,
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

        // Squares attacked by the opposite side
        bitboard attacked = 0;
    };

    class BoardPosition;

    class BoardHash
    {
    public:
        BoardHash(BoardPosition &pos);
        BoardHash(BoardHash *&prev);

        // Data for undoing a move
        int castling_rights;
        bpos en_passant;
        uint32_t ply; // Halfmove

        size_t key;
        BoardHash* prev;

        bool operator==(const BoardHash &other) const;
    };

    /*
    Bit numeration in bitboards:
    63 62 61 60 59 58 57 56
    ...
    7  6  5  4  3  2  1  0
    */
    struct BoardPosition
    {
        ~BoardPosition();

        bitboard pieces[PIECE_NB];
        Piece squares[64];

        // Determines current side to move
        // 0 - White, 1 - Black
        Color side_to_move;
        unsigned int fullmove;

        movgen::BoardHash *hash = new movgen::BoardHash(*this);
        PositionInfo *info = nullptr;
    };

    inline const char *fen_regex_string =
        "\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)"
        "\\s*([b|w])\\s*([K|Q|k|q]{1,4}|-)\\s*(-|[a-h][1-8])\\s*(\\d+\\s\\d+){0,1}\\s*$";

    BoardPosition board_from_fen(std::string fen);
    std::string board_to_fen(BoardPosition &pos);

    struct Move
    {
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

        MoveType get_type() const;
        Piece get_captured() const;
        PieceType get_promoted() const;
    };

    Piece get_piece(BoardPosition &b_pos, bpos pos);

    constexpr movgen::PieceType get_piece_type(movgen::Piece piece)
    {
        switch (piece)
        {
        case movgen::B_KING:
        case movgen::W_KING:
            return movgen::KING;
        case movgen::B_QUEEN:
        case movgen::W_QUEEN:
            return movgen::QUEEN;
        case movgen::B_ROOK:
        case movgen::W_ROOK:
            return movgen::ROOK;
        case movgen::B_BISHOP:
        case movgen::W_BISHOP:
            return movgen::BISHOP;
        case movgen::B_KNIGHT:
        case movgen::W_KNIGHT:
            return movgen::KNIGHT;
        case movgen::B_PAWN:
        case movgen::W_PAWN:
            return movgen::PAWN;
        default:
            return movgen::NO_PIECE_TYPE;
        }
    }

    PieceType get_piece_type(BoardPosition &b_pos, bpos pos);

    movgen::Color get_piece_color(movgen::Piece piece);

    constexpr movgen::Piece get_piece_from_type(movgen::PieceType type, movgen::Color c)
    {
        switch (type)
        {
        case movgen::KING:
            return c == movgen::WHITE ? movgen::W_KING : movgen::B_KING;
        case movgen::QUEEN:
            return c == movgen::WHITE ? movgen::W_QUEEN : movgen::B_QUEEN;
        case movgen::ROOK:
            return c == movgen::WHITE ? movgen::W_ROOK : movgen::B_ROOK;
        case movgen::BISHOP:
            return c == movgen::WHITE ? movgen::W_BISHOP : movgen::B_BISHOP;
        case movgen::KNIGHT:
            return c == movgen::WHITE ? movgen::W_KNIGHT : movgen::B_KNIGHT;
        case movgen::PAWN:
            return c == movgen::WHITE ? movgen::W_PAWN : movgen::B_PAWN;
        default:
            return movgen::NO_PIECE;
        }
    }
}; // namespace movgen

template <>
struct std::hash<movgen::BoardHash>
{
    size_t operator()(movgen::BoardHash const &p) const noexcept;
};

// Define a hash function for BoardPosition
template <>
struct std::hash<movgen::BoardPosition>
{
    size_t operator()(movgen::BoardPosition const &p) const noexcept;
};

#endif
