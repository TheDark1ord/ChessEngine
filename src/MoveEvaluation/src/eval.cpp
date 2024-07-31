#include "../headers/eval.h"

float eval(movgen::BoardPosition &pos)
{
    float piece_eval = 0;
    for (movgen::Piece piece : pos.squares)
    {
        bool color = movgen::get_piece_color(piece);
        float value = piece_val(movgen::get_piece_type(piece));

        // BLACK -- neg value
        // WHITE -- pos value
        piece_eval += (color - 1) * value;
    }
    
    return piece_eval;
}
