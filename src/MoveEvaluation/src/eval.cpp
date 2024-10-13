#include "../headers/eval.h"

float eval(movgen::BoardPosition& pos)
{
	float piece_eval = 0.0f;
	for(movgen::Piece piece : pos.squares)
	{
		if(piece == movgen::Piece::NO_PIECE)
			continue;

		bool color = movgen::get_piece_color(piece);
		float value = piece_val(movgen::get_piece_type(piece));

		piece_eval += (color == movgen::Color::WHITE) ? value : -value;
	}

	float piece_mobility = 0.0f;
	// Можно добавить несколько bitbiards для мобильности на разные значения eval
	// bitboard для 0.1 eval, 0.2 eval и т.п.
	piece_mobility +=
		static_cast<float>(
			bitb::bit_count(pos.info->w_piece_attacks & ~(pos.pieces[movgen::WHITE_PIECES])) -
			bitb::bit_count(pos.info->b_piece_attacks & ~(pos.pieces[movgen::BLACK_PIECES]))) *
		0.05f;
	// Penalize king mobility
	piece_mobility -=
		static_cast<float>(
			bitb::bit_count(pos.info->w_king_attacks & ~(pos.pieces[movgen::WHITE_PIECES])) -
			bitb::bit_count(pos.info->b_king_attacks & ~(pos.pieces[movgen::BLACK_PIECES]))) *
		0.05f;

	//Emphasize center control
	constexpr bitboard center = 0x3c18000000; // Valued at 0.1 points
	float center_control =
		static_cast<float>(
			bitb::bit_count(center & (pos.info->w_pawn_attacks | pos.info->w_piece_attacks))) *
			0.1 -
		static_cast<float>(bitb::bit_count(
			bitb::flip_vertical(center) & (pos.info->b_pawn_attacks | pos.info->b_piece_attacks))) *
			0.1f;  
	
	return piece_eval + piece_mobility + center_control;
}
