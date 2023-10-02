#ifndef TYPES_H
#define TYPES_H

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
        // Double pawn move(1 bit) -- used for faster detection to set the en passant square
        // Castling(2 bits) -- none, short, long
        //
        // Note: there is no validity check
        uint16_t move_data;

        /// @brief
        /// Constructs "Move" struct instance, 4 last params used to construct Move.move_data parameter
        /// @param piece
        /// @param from
        /// @param to
        /// @param capture see Move.move_data
        /// @param promotion see Move.move_data comment
        /// @param double_move if a pawn moved two squares forward
        /// @param castling see Move.move_data comment
        Move(Piece piece, bpos from, bpos to,
            unsigned char capturen = 0, unsigned char promotion = 0,
            bool double_move = 0, unsigned char castling = 0);
    };

    /*
        Bit numeration:
        63 62 61 60 59 58 57 56
        ...
        7  6  5  4  3  2  1  0
    */
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

    inline bool read_bit(bitboard& b, bpos pos);
    inline void set_bit(bitboard* b, bpos pos);
    inline void cleat_bit(bitboard* b, bpos pos);
    inline void flip_bit(bitboard* b, bpos pos);

    /// @brief converts a bitboard into a sequense of values between 0 and 63, that
    /// correspond to positions of set bits in the bitboard
    inline std::vector<bpos> bitscan(bitboard board);

    /// @brief scans all of the bitboards on the given board and return the bitboard for
    /// which there is a bit set in a given position
    /// @param pos position to scan
    /// @param color if color of the piece is known, scan only bitboard of corresponding color
    ///     0 - scan all colors, 1 - scan only white, 2 - scan only black
    /// @return piece, which is in given board cell
    Piece get_piece(BoardPosition& b_pos, bpos pos, unsigned char color = 0);

    class BoardHash
    {
        BoardHash(BoardPosition& pos);
        // Define a custom hash operator to use in std::set
        size_t operator()(BoardHash const& h) const;

        size_t hash;
        // Number of times this position was reached
        unsigned int reached = 1;
    };
};

// Define a hash function for BoardPosition
template<>
struct std::hash<movgen::BoardPosition>
{
    size_t operator()(movgen::BoardPosition const& p) const noexcept;
};

#endif