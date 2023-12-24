#include "../include/MovgenTypes.h"

movgen::Move::Move(Piece piece, bpos from, bpos to, unsigned char capture, unsigned char promotion, bool double_move, unsigned char castling)
    :piece(piece), from(from), to(to)
{
    this->move_data = 0;

    this->move_data |= (unsigned int)piece & 0x0F;
    this->move_data |= promotion & 0x0F << 4;
    this->move_data |= double_move << 5;
    this->move_data |= castling & 0x03 << 4;
}