#include "../headers/eval.h"
#include "MovgenTypes.h"

constexpr bitboard center = 0x1818000000;

float eval(movgen::BoardPosition& pos)
{
	// (pos.side_to_move * 2 - 1) -- map side to move to [-1;+1]
	return (_eval_white(pos) - _eval_black(pos)) * (pos.side_to_move * 2 - 1);
}

static float _eval_white(movgen::BoardPosition& pos)
{
	float piece_eval = 0.0f;
	for(uint16_t piece_type = movgen::Piece::W_KING; piece_type <= movgen::Piece::W_PAWN; piece_type++)
	{
		piece_eval += static_cast<float>(bitb::bit_count(pos.pieces[piece_type])) *
					  piece_val(movgen::get_piece_type(static_cast<movgen::Piece>(piece_type)));
	}

	float piece_mobility = 0.0f;
	piece_mobility +=
		static_cast<float>(bitb::bit_count(pos.info->w_piece_attacks & ~(pos.pieces[movgen::WHITE_PIECES]))) * 0.05f;
	// Penalize king mobility
	piece_mobility -=
		static_cast<float>(bitb::bit_count(pos.info->w_king_attacks & ~(pos.pieces[movgen::WHITE_PIECES]))) * 0.05f;

	float center_control = static_cast<float>(bitb::bit_count(center & (pos.pieces[movgen::W_PAWN]))) * 2.1f;
	center_control += static_cast<float>(bitb::bit_count(center & pos.info->w_piece_attacks)) * 0.05f;

	if(pos.side_to_move == movgen::WHITE)
		return piece_eval + piece_mobility + center_control + 0.25; //Tempo score 
	else
		return piece_eval + piece_mobility + center_control;
}

static float _eval_black(movgen::BoardPosition& pos)
{
	float piece_eval = 0.0f;
	for(uint16_t piece_type = movgen::Piece::B_KING; piece_type <= movgen::Piece::B_PAWN; piece_type++)
	{
		piece_eval += static_cast<float>(bitb::bit_count(pos.pieces[piece_type])) *
					  piece_val(movgen::get_piece_type(static_cast<movgen::Piece>(piece_type)));
	}

	float piece_mobility = 0.0f;
	piece_mobility +=
		static_cast<float>(bitb::bit_count(pos.info->b_piece_attacks & ~(pos.pieces[movgen::BLACK_PIECES]))) * 0.05f;
	// Penalize king mobility
	piece_mobility -=
		static_cast<float>(bitb::bit_count(pos.info->b_king_attacks & ~(pos.pieces[movgen::BLACK_PIECES]))) * 0.05f;

	float center_control = static_cast<float>(bitb::bit_count(center & (pos.pieces[movgen::B_PAWN]))) * 2.1f;
	center_control += static_cast<float>(bitb::bit_count(center & pos.info->b_piece_attacks)) * 0.05f;

	if(pos.side_to_move == movgen::BLACK)
		return piece_eval + piece_mobility + center_control + 0.25; //Tempo score
	else
		return piece_eval + piece_mobility + center_control;
}