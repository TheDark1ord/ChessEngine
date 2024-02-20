#include "../include/MovgenTypes.h"

std::vector<bpos> movgen::bitscan(bitboard board)
{
    // https://www.chessprogramming.org/BitScan#Bitscan_by_Modulo
    static constexpr int lookup67[68] = {
        64,  0,  1, 39,  2, 15, 40, 23,
        3, 12, 16, 59, 41, 19, 24, 54,
        4, -1, 13, 10, 17, 62, 60, 28,
        42, 30, 20, 51, 25, 44, 55, 47,
        5, 32, -1, 38, 14, 22, 11, 58,
        18, 53, 63,  9, 61, 27, 29, 50,
        43, 46, 31, 37, 21, 57, 52,  8,
        26, 49, 45, 36, 56,  7, 48, 35,
        6, 34, 33, -1
    };

    std::vector<bpos> set_bits;
    // TODO: Test optimal reserve number or even better update to better algorithm
    set_bits.reserve(8);

    while (board != 0)
    {
        set_bits.push_back(lookup67[(board & (~board + 1)) % 67]);
        board &= board - 1;
    }

    return set_bits;
}

movgen::Piece movgen::get_piece(BoardPosition& b_pos, bpos pos, unsigned char color)
{
    return b_pos.squares[pos];
}

    if (color != 1)
    {
        return_piece = b_pos.b_kings & mask ? Piece::B_KING : return_piece;
        return_piece = b_pos.b_queens & mask ? Piece::B_QUEEN : return_piece;
        return_piece = b_pos.b_rooks & mask ? Piece::B_ROOK : return_piece;
        return_piece = b_pos.b_bishops & mask ? Piece::B_BISHOP : return_piece;
        return_piece = b_pos.b_knights & mask ? Piece::B_KNIGHT : return_piece;
        return_piece = b_pos.b_pawns & mask ? Piece::B_PAWN : return_piece;
    }

    return return_piece;
}

bitboard* movgen::get_bitboard_for_piece(BoardPosition& b_pos, Piece piece)
{
    switch (piece)
    {
    case Piece::W_KING:
        return &b_pos.w_kings;
    case Piece::B_KING:
        return &b_pos.b_kings;

    case Piece::W_QUEEN:
        return &b_pos.w_queens;
    case Piece::B_QUEEN:
        return &b_pos.b_queens;

    case Piece::W_ROOK:
        return &b_pos.w_rooks;
    case Piece::B_ROOK:
        return &b_pos.b_rooks;

    case Piece::W_BISHOP:
        return &b_pos.w_bishops;
    case Piece::B_BISHOP:
        return &b_pos.b_bishops;

    case Piece::W_KNIGHT:
        return &b_pos.w_knights;
    case Piece::B_KNIGHT:
        return &b_pos.b_knights;

    case Piece::W_PAWN:
        return &b_pos.w_pawns;
    case Piece::B_PAWN:
        return &b_pos.b_pawns;

    default:
        return nullptr;
    }
}

movgen::Move::Move(Piece piece, bpos from, bpos to, unsigned char capture, unsigned char promotion, bool double_move, unsigned char castling)
    :piece(piece), from(from), to(to)
{
    this->move_data = 0;

    this->move_data |= (unsigned int)piece & 0x0F;
    this->move_data |= promotion & 0x0F << 4;
    this->move_data |= double_move << 5;
    this->move_data |= castling & 0x03 << 4;
}