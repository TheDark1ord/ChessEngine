#include "../headers/eval.h"
#include "MovgenTypes.h"

constexpr bitboard center = 0x1818000000;

float eval(movgen::BoardPosition& pos)
{
	// (pos.side_to_move * 2 - 1) -- map side to move to [-1;+1]
	// This is needed for negamax framework
	return (_eval_side<movgen::WHITE>(pos) - _eval_side<movgen::BLACK>(pos)) * (pos.side_to_move * 2 - 1);
}

template<movgen::Color col>
float _eval_side(const movgen::BoardPosition& pos)
{
	constexpr int8_t them = col == movgen::WHITE ? 0 : 8;
	constexpr int8_t us = col == movgen::WHITE ? 8 : 0;

	const bitboard& piece_attacks = col == movgen::WHITE ?
		pos.info->w_piece_attacks : pos.info->b_piece_attacks;
	const bitboard& pawn_attacks = col == movgen::WHITE ?
		pos.info->w_pawn_attacks : pos.info->b_pawn_attacks;
	const bitboard& king_attacks = col == movgen::WHITE ?
		pos.info->w_king_attacks : pos.info->b_king_attacks;

	float piece_eval = 0.0f;
	for(uint16_t piece_type = movgen::Piece::B_KING + us; piece_type <= movgen::Piece::B_PAWN + us; piece_type++)
		piece_eval += static_cast<float>(bitb::bit_count(pos.pieces[piece_type])) *
					  piece_val(movgen::get_piece_type(static_cast<movgen::Piece>(piece_type)));

	float piece_mobility = 0.0f;
	piece_mobility += static_cast<float>(
		bitb::bit_count(pos.info->w_piece_attacks & ~(pos.pieces[movgen::BLACK_PIECES + us]))) * 0.025f;
	// Penalize king mobility
	piece_mobility -= static_cast<float>
		(bitb::bit_count(pos.info->w_king_attacks & ~(pos.pieces[movgen::BLACK_PIECES + us]))) * 0.1f;

	float center_control = static_cast<float>(bitb::bit_count(
				center & (pos.pieces[movgen::BLACK_PIECES + us]))) * 0.1f;
	center_control += static_cast<float>(bitb::bit_count(center & piece_attacks)) * 0.05f;

	//And finnaly add piece square tables
	float pst = 0.0f;
	for(uint16_t piece_type = movgen::PieceType::KING; piece_type <= movgen::PieceType::PAWN; piece_type++)
		for(bpos square : bitb::BitscanIterator(pos.pieces[piece_type + us]))
			pst += col == movgen::WHITE ?
				piece_square_tables[piece_type - 1][63 - square] :
				piece_square_tables[piece_type - 1][square];
	pst *= 0.01;

	if(col == pos.side_to_move)
		return piece_eval + piece_mobility + center_control + pst + 0.25; //Tempo score
	else
		return piece_eval + piece_mobility + center_control + pst;
}
