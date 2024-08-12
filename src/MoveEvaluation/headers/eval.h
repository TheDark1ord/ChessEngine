#ifndef EVAL_H
#define EVAL_H

#include "MovgenTypes.h"
#include <cmath>

static constexpr float piece_val(movgen::PieceType piece) {
  switch (piece) {
  case movgen::KING:
    return 0;
  case movgen::QUEEN:
    return 8.0;
  case movgen::ROOK:
    return 5.0;
  case movgen::BISHOP:
    return 3.2;
  case movgen::KNIGHT:
    return 3.0;
  case movgen::PAWN:
    return 1.0;
  default:
    return INFINITY;
  }
}

// Range: -40 -- +40
// (-)n*e+3 -- mate in n moves
float eval(movgen::BoardPosition &pos);

#endif
