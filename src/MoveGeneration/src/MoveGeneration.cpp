#include "../include/MoveGeneration.h"
#include "../include/MagicNumbers.h"
#include "../include/Zobrist.h"
#include <cassert>

#define PIECE_MOVE_INSTANCE(piece, type)                                                                                    \
	template void movgen::generate_piece_moves<piece, type>(                                                                \
		bpos piece_pos, BoardPosition & pos, movgen::Color c, std::vector<Move> * generated)

#define PAWN_MOVE_INSTANCE(piece, type)                                                                                     \
	template void movgen::generate_pawn_moves<piece, type>(BoardPosition & pos, std::vector<Move> * generated)

#define ALL_MOVE_INSTANCE(piece, type)                                                                                      \
	template void movgen::generate_all_moves<piece, type>(BoardPosition & pos, std::vector<Move> * generated)

#define MAKE_MOVE_INSTANCE(type)                                                                                            \
	template void movgen::make_move<type>(                                                                                  \
		movgen::BoardPosition * pos, movgen::Move & move, std::vector<movgen::Move> * new_moves)

bitboard movgen::knight_attacks[64];
bitboard movgen::king_attacks[64];
std::atomic_bool movgen::initialized = false;

void movgen::init()
{
	zobrist::init();

	const std::tuple<int, int> king_moves[]{
		{-1, -1},
		{0, -1},
		{1, -1},
		{-1, 0},
		{1, 0},
		{-1, 1},
		{0, 1},
		{1, 1},
	};

	const std::tuple<int, int> knight_moves[]{
		{-1, -2},
		{1, -2},
		{-2, -1},
		{2, -1},
		{-1, 2},
		{1, 2},
		{-2, 1},
		{2, 1},
	};

	for(uint16_t y = 0; y < 8; y++)
	{
		for(uint16_t x = 0; x < 8; x++)
		{
			bitboard king_board = 0;
			bitboard knight_board = 0;

			for(auto offset : king_moves)
			{
				int ky = y + std::get<0>(offset);
				int kx = x + std::get<1>(offset);

				if(ky >= 0 && ky < 8 && kx >= 0 && kx < 8)
					king_board |= 1ull << (ky * 8 + kx);
			}
			for(auto offset : knight_moves)
			{
				int ky = y + std::get<0>(offset);
				int kx = x + std::get<1>(offset);

				if(ky >= 0 && ky < 8 && kx >= 0 && kx < 8)
					knight_board |= 1ull << (ky * 8 + kx);
			}

			king_attacks[y * 8 + x] = king_board;
			knight_attacks[y * 8 + x] = knight_board;
		}
	}

	initialized = true;
}

template <movgen::PieceType type, movgen::GenType gen_type>
void movgen::generate_piece_moves(bpos piece_pos, BoardPosition& pos, movgen::Color c, std::vector<Move>* generated)
{
	const unsigned int us = 8 * c;
	const unsigned int them = 8 - us;

	if(type == movgen::KING)
	{
		if(gen_type == movgen::GenType::CASTLING || gen_type == movgen::GenType::ALL_MOVES)
		{
			const auto short_castle = c == movgen::WHITE ? movgen::WHITE_SHORT : movgen::BLACK_SHORT;
			const auto long_castle = c == movgen::WHITE ? movgen::WHITE_LONG : movgen::BLACK_LONG;

			if(pos.hash->castling_rights & short_castle)
				// Check if no pieces are blocking the castling
				if(!(bitb::Between_in[piece_pos][piece_pos - 3] & pos.pieces[ALL_PIECES]))
					// Check if there is a rook
					if(pos.pieces[B_ROOK + us] & (bitb::sq_rank(piece_pos) & bitb::File[7]))
						generated->push_back(
							Move(get_piece_from_type(movgen::KING, c), piece_pos, piece_pos - 2, 0, 0, 0, 0, 1));

			if(pos.hash->castling_rights & long_castle)
				// Check if no pieces are blocking the castling
				if(!(bitb::Between_in[piece_pos][piece_pos + 4] & pos.pieces[ALL_PIECES]))
					// Check if there is a rook
					if(pos.pieces[B_ROOK + us] & (bitb::sq_rank(piece_pos) & bitb::File[0]))
						generated->push_back(
							Move(get_piece_from_type(movgen::KING, c), piece_pos, piece_pos + 2, 0, 0, 0, 0, 2));
		}

		if(gen_type != movgen::GenType::CASTLING)
		{
			bitboard moves =
				get_pseudo_attacks<movgen::KING>(piece_pos, pos.pieces[ALL_PIECES]) & ~pos.pieces[BLACK_PIECES + us];

			if(gen_type != movgen::GenType::QUIETS)
			{
				bitboard captures = moves & pos.pieces[BLACK_PIECES + them];

				for(auto move_to : bitb::bitscan(captures))
					generated->push_back(
						Move(get_piece_from_type(movgen::KING, c), piece_pos, move_to, get_piece(pos, move_to)));
			}
			if(gen_type != movgen::GenType::CAPTURES)
			{
				bitboard quiet = moves & ~pos.pieces[BLACK_PIECES + them];

				for(auto move_to : bitb::bitscan(quiet))
					generated->push_back(Move(get_piece_from_type(movgen::KING, c), piece_pos, move_to));
			}
		}
	}
	else
	{
		bitboard moves = get_pseudo_attacks<type>(piece_pos, pos.pieces[ALL_PIECES]) & ~pos.pieces[BLACK_PIECES + us];

		if(gen_type != movgen::GenType::QUIETS)
		{
			bitboard captures = moves & pos.pieces[BLACK_PIECES + them];
			for(auto move_to : bitb::bitscan(captures))
				generated->push_back(Move(get_piece_from_type(type, c), piece_pos, move_to, get_piece(pos, move_to)));
		}
		if(gen_type != movgen::GenType::CAPTURES)
		{
			bitboard quiet = moves & ~pos.pieces[BLACK_PIECES + them];
			for(auto move_to : bitb::bitscan(quiet))
				generated->push_back(Move(get_piece_from_type(type, c), piece_pos, move_to));
		}
	}
}

PIECE_MOVE_INSTANCE(movgen::QUEEN, movgen::GenType::ALL_MOVES);
PIECE_MOVE_INSTANCE(movgen::QUEEN, movgen::GenType::QUIETS);
PIECE_MOVE_INSTANCE(movgen::QUEEN, movgen::GenType::CAPTURES);
PIECE_MOVE_INSTANCE(movgen::ROOK, movgen::GenType::ALL_MOVES);
PIECE_MOVE_INSTANCE(movgen::ROOK, movgen::GenType::QUIETS);
PIECE_MOVE_INSTANCE(movgen::ROOK, movgen::GenType::CAPTURES);
PIECE_MOVE_INSTANCE(movgen::BISHOP, movgen::GenType::ALL_MOVES);
PIECE_MOVE_INSTANCE(movgen::BISHOP, movgen::GenType::QUIETS);
PIECE_MOVE_INSTANCE(movgen::BISHOP, movgen::GenType::CAPTURES);
PIECE_MOVE_INSTANCE(movgen::KNIGHT, movgen::GenType::ALL_MOVES);
PIECE_MOVE_INSTANCE(movgen::KNIGHT, movgen::GenType::QUIETS);
PIECE_MOVE_INSTANCE(movgen::KNIGHT, movgen::GenType::CAPTURES);
PIECE_MOVE_INSTANCE(movgen::KING, movgen::GenType::ALL_MOVES);
PIECE_MOVE_INSTANCE(movgen::KING, movgen::GenType::QUIETS);
PIECE_MOVE_INSTANCE(movgen::KING, movgen::GenType::CAPTURES);
PIECE_MOVE_INSTANCE(movgen::KING, movgen::GenType::CASTLING);

template <movgen::Color color, movgen::GenType gen_type>
void movgen::generate_pawn_moves(BoardPosition& pos, std::vector<Move>* generated)
{
	constexpr unsigned int us = 8 * color;
	constexpr unsigned int them = 8 - us;
	constexpr bitboard rank3 = (color == WHITE ? bitb::Rank[2] : bitb::Rank[5]);
	constexpr bitboard rank7 = (color == WHITE ? bitb::Rank[6] : bitb::Rank[1]);
	constexpr Piece piece = (color == WHITE ? W_PAWN : B_PAWN);

	constexpr bitb::Direction forward = (color == WHITE ? bitb::UP : bitb::DOWN);
	constexpr bitb::Direction back = (color == WHITE ? bitb::DOWN : bitb::UP);
	constexpr bitb::Direction forward_left = static_cast<bitb::Direction>(forward + bitb::LEFT);
	constexpr bitb::Direction forward_right = static_cast<bitb::Direction>(forward + bitb::RIGHT);

	const bitboard prom_pawns = pos.pieces[B_PAWN + us] & rank7;
	const bitboard not_prom_pawns = pos.pieces[B_PAWN + us] & ~rank7;

	if(gen_type == movgen::GenType::PROMOTIONS || gen_type == movgen::GenType::ALL_MOVES)
	{

		if(prom_pawns)
		{
			bitboard left_capture_prom = bitb::shift<forward_left>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
			bitboard right_capture_prom = bitb::shift<forward_right>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
			bitboard push_prom = bitb::shift<forward>(prom_pawns) & ~pos.pieces[ALL_PIECES];

			for(auto move_to : bitb::bitscan(left_capture_prom))
				_make_promotions<color, forward_left>(generated, move_to, get_piece(pos, move_to));
			for(auto move_to : bitb::bitscan(right_capture_prom))
				_make_promotions<color, forward_right>(generated, move_to, get_piece(pos, move_to));
			for(auto move_to : bitb::bitscan(push_prom))
				_make_promotions<color, forward>(generated, move_to, 0);
		}
	}

	if(gen_type == movgen::GenType::CAPTURES || gen_type == movgen::GenType::ALL_MOVES)
	{
		// En passant
		if(pos.hash->en_passant != 0)
		{
			bitboard candidate_pawns = (bitb::shift<bitb::LEFT>(1ull << (pos.hash->en_passant + back)) |
										bitb::shift<bitb::RIGHT>(1ull << (pos.hash->en_passant + back))) &
									   pos.pieces[B_PAWN + us];

			for(auto move_from : bitb::bitscan(candidate_pawns))
				generated->push_back(Move(
					piece, move_from, pos.hash->en_passant, get_piece_from_type(movgen::PAWN, color), 0, false, true, 0));
		}

		// Captures
		bitboard left_capture = bitb::shift<forward_left>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];
		bitboard right_capture = bitb::shift<forward_right>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];

		for(auto move_to : bitb::bitscan(left_capture))
			generated->push_back(Move(piece, move_to - forward - bitb::LEFT, move_to, get_piece(pos, move_to)));
		for(auto move_to : bitb::bitscan(right_capture))
			generated->push_back(Move(piece, move_to - forward - bitb::RIGHT, move_to, get_piece(pos, move_to)));
	}

	if(gen_type == movgen::GenType::QUIETS || gen_type == movgen::GenType::ALL_MOVES)
	{
		// Single and double moves
		bitboard s = bitb::shift<forward>(not_prom_pawns) & ~pos.pieces[ALL_PIECES];
		bitboard d = bitb::shift<forward>(s & rank3) & ~pos.pieces[ALL_PIECES];

		for(auto move_to : bitb::bitscan(d))
			generated->push_back(Move(piece, move_to - forward * 2, move_to, 0, 0, true, false, 0));
		for(auto move_to : bitb::bitscan(s))
			generated->push_back(Move(piece, move_to - forward, move_to));
	}
}

PAWN_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::ALL_MOVES);
PAWN_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::QUIETS);
PAWN_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::CAPTURES);
PAWN_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::PROMOTIONS);
PAWN_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::ALL_MOVES);
PAWN_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::QUIETS);
PAWN_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::CAPTURES);
PAWN_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::PROMOTIONS);

template <>
bitboard movgen::get_pseudo_attacks<movgen::KING>(bpos piece_pos, bitboard blocker)
{
	return king_attacks[piece_pos];
}

template <>
bitboard movgen::get_pseudo_attacks<movgen::QUEEN>(bpos piece_pos, bitboard blocker)
{
	return movgen::get_rook_attacks(piece_pos, blocker) | movgen::get_bishop_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, bitboard blocker)
{
	return movgen::get_rook_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, bitboard blocker)
{
	return movgen::get_bishop_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, bitboard blocker)
{
	return knight_attacks[piece_pos];
}

template <movgen::Color them_c>
void movgen::get_checkers(BoardPosition& pos, PositionInfo* info)
{
	constexpr unsigned int them = 8 * them_c;
	constexpr unsigned int us = 8 - them;
	const bpos king_pos = bitb::pop_lsb(pos.pieces[B_KING + us]);
	bitboard& piece_attacks = pos.side_to_move == movgen::WHITE ? info->b_piece_attacks : info->w_piece_attacks;

	constexpr bitb::Direction back = us == movgen::WHITE ? bitb::DOWN : bitb::UP;

	bitboard king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES]);
	bitboard bishop_checkers = king_sliding & (pos.pieces[B_BISHOP + them] | pos.pieces[B_QUEEN + them]);

	// Prevent the king from going alongside the attacking ray
	for(auto attacker : bitb::bitscan(bishop_checkers))
		piece_attacks |= get_pseudo_attacks<BISHOP>(attacker, pos.pieces[ALL_PIECES] ^ pos.pieces[B_KING + us]);

	king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES]);
	bitboard rook_checkers = king_sliding & (pos.pieces[B_ROOK + them] | pos.pieces[B_QUEEN + them]);

	// Prevent the king from going alongside the attacking ray
	for(auto attacker : bitb::bitscan(rook_checkers))
		piece_attacks |= get_pseudo_attacks<ROOK>(attacker, pos.pieces[ALL_PIECES] ^ pos.pieces[B_KING + us]);

	// Check for knights
	bitboard king_jump = get_pseudo_attacks<KNIGHT>(king_pos, pos.pieces[ALL_PIECES]);
	bitboard knight_checkers = king_jump & pos.pieces[B_KNIGHT + them];

	info->blockers |= knight_checkers;

	// Check for pawns
	constexpr bitb::Direction forward_left =
		static_cast<bitb::Direction>((them_c == movgen::WHITE ? bitb::DOWN : bitb::UP) + bitb::LEFT);
	constexpr bitb::Direction forward_right =
		static_cast<bitb::Direction>((them_c == movgen::WHITE ? bitb::DOWN : bitb::UP) + bitb::RIGHT);
	bitboard king_move =
		bitb::shift<forward_left>(pos.pieces[B_KING + us]) | bitb::shift<forward_right>(pos.pieces[B_KING + us]);
	bitboard pawn_checkers = king_move & pos.pieces[B_PAWN + them];

	// If it is possible to capture the checking pawn with en_passant, add en passant square to blockers board
	if(pos.hash->en_passant > 0 && (bitb::shift<back>(pawn_checkers) & bitb::sq_bitb(pos.hash->en_passant)))
		bitb::set_bit(&info->blockers, pos.hash->en_passant);

	info->checkers |= bishop_checkers;
	info->checkers |= rook_checkers;
	info->checkers |= knight_checkers;
	info->checkers |= pawn_checkers;

	info->checks_num = bitb::bit_count(info->checkers);

	if(info->checkers > 0)
	{ // Using pop_lsb here because this bitboard should not matter in case of more than 1 checker
		info->blockers |= bitb::Between[king_pos][bitb::pop_lsb(info->checkers)];
		info->blockers |= knight_checkers;
		info->blockers |= pawn_checkers;
	}
}

template <movgen::Color them_c>
void movgen::get_pinners(BoardPosition& pos, PositionInfo* info)
{
	constexpr unsigned int them = 8 * them_c;
	constexpr unsigned int us = 8 - them;
	// Direction in which a pawn advances
	constexpr bitb::Direction forward = them_c == BLACK ? bitb::UP : bitb::DOWN;
	constexpr bitb::Direction back = them_c == BLACK ? bitb::DOWN : bitb::UP;
	constexpr bitb::Direction back_left = static_cast<bitb::Direction>(back + bitb::LEFT);
	constexpr bitb::Direction back_right = static_cast<bitb::Direction>(back + bitb::RIGHT);

	bpos king_pos = bitb::pop_lsb(pos.pieces[B_KING + us]);

	bitboard king_sliding = get_pseudo_attacks<QUEEN>(king_pos, pos.pieces[ALL_PIECES]);
	bitboard candidates = king_sliding & pos.pieces[BLACK_PIECES + us];

	king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
	bitboard pinners = king_sliding & pos.pieces[BLACK_PIECES + them];
	for(bpos pin : bitb::bitscan(pinners))
	{
		if(get_piece(pos, pin) == (B_BISHOP + them) || get_piece(pos, pin) == (B_QUEEN + them))
		{
			bitboard candidate = pos.pieces[BLACK_PIECES + us] & bitb::Between_in[king_pos][pin];

			if(candidate == 0)
				continue;

			PositionInfo::Pin new_pin;
			new_pin.pinned = bitb::pop_lsb(candidate);
			new_pin.pinner = pin;
			new_pin.mask = bitb::Between[king_pos][new_pin.pinner];

			info->pins.push_back(new_pin);
			info->pin_board |= 1ull << new_pin.pinned;
		}
	}

	king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
	pinners = king_sliding & pos.pieces[BLACK_PIECES + them];
	for(bpos pin : bitb::bitscan(pinners))
	{
		if(get_piece(pos, pin) == (B_ROOK + them) || get_piece(pos, pin) == (B_QUEEN + them))
		{
			bitboard candidate = pos.pieces[BLACK_PIECES + us] & bitb::Between_in[king_pos][pin];

			if(candidate == 0)
				continue;

			PositionInfo::Pin new_pin;

			new_pin.pinned = bitb::pop_lsb(candidate);
			new_pin.pinner = pin;
			new_pin.mask = bitb::Between[king_pos][pin];

			info->pins.push_back(new_pin);
			info->pin_board |= 1ull << new_pin.pinned;
		}
	}

	// It is possible that this code detects a check instead of a pin, but in
	// that case we would not be able to do en passant anyway Check for en
	// passant pin
	if(pos.hash->en_passant != 0)
	{
		bitboard enp_board = bitb::sq_bitb(pos.hash->en_passant);
		// If the king is not on the same rank as the pawns, en passant pin is impossible
		if(bitb::sq_rank(king_pos) & bitb::shift<back>(enp_board))
		{
			bitboard en_passant_pawn = bitb::shift<back>(enp_board);

			bitboard left_pawn = bitb::shift<back_left>(enp_board);
			if(left_pawn & pos.pieces[B_PAWN + us])
			{
				bitboard wo_left = pos.pieces[ALL_PIECES] ^ left_pawn ^ en_passant_pawn;
				bitboard pinner_mask =
					movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_left) & bitb::sq_rank(king_pos);
				if(pinner_mask & (pos.pieces[B_QUEEN + them] | pos.pieces[B_ROOK + them]))
					info->en_passant_pin = 1;
			}

			bitboard right_pawn = bitb::shift<back_right>(enp_board);
			if(&pos.pieces[B_PAWN + us])
			{
				bitboard wo_right = pos.pieces[ALL_PIECES] ^ right_pawn ^ en_passant_pawn;
				bitboard pinner_mask =
					movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_right) & bitb::sq_rank(king_pos);
				if(pinner_mask & (pos.pieces[B_QUEEN + them] | pos.pieces[B_ROOK + them]))
					info->en_passant_pin = 1;
			}
		}
	}
}

template <movgen::Color color>
void movgen::get_attacked(BoardPosition& pos, PositionInfo* info)
{
	constexpr unsigned int us = 8 * color;
	constexpr bitb::Direction forward = color == WHITE ? bitb::UP : bitb::DOWN;
	constexpr bitb::Direction forward_left = static_cast<bitb::Direction>(forward + bitb::LEFT);
	constexpr bitb::Direction forward_right = static_cast<bitb::Direction>(forward + bitb::RIGHT);

	bitboard& piece_attacks = color == movgen::WHITE ? info->w_piece_attacks : info->b_piece_attacks;
	bitboard& pawn_attacks = color == movgen::WHITE ? info->w_pawn_attacks : info->b_pawn_attacks;
	bitboard& king_attacks = color == movgen::WHITE ? info->w_king_attacks : info->b_king_attacks;

#define get_attacks(attacks, piece)                                                                                         \
	for(bpos p_pos : bitb::bitscan(pos.pieces[piece + us]))                                                                 \
		attacks |= get_pseudo_attacks<piece>(p_pos, pos.pieces[ALL_PIECES]);

	get_attacks(king_attacks, KING);
	get_attacks(piece_attacks, QUEEN);
	get_attacks(piece_attacks, ROOK);
	get_attacks(piece_attacks, BISHOP);
	get_attacks(piece_attacks, KNIGHT);
	pawn_attacks |= bitb::shift<forward_left>(pos.pieces[B_PAWN + us]) | bitb::shift<forward_right>(pos.pieces[B_PAWN + us]);
}

template <movgen::Color color, movgen::GenType gen_type>
void movgen::generate_all_moves(BoardPosition& pos, std::vector<movgen::Move>* moves)
{
	constexpr movgen::Color col_them = movgen::Color(!color);

	// TODO: Test optimal reserve number
	moves->reserve(10);

	switch(gen_type)
	{
	case movgen::GenType::ALL_MOVES:
	case movgen::GenType::QUIETS:
	case movgen::GenType::CAPTURES:
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::KING, color)]))
			generate_piece_moves<movgen::KING, gen_type>(piece_pos, pos, color, moves);
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::QUEEN, color)]))
			generate_piece_moves<movgen::QUEEN, gen_type>(piece_pos, pos, color, moves);
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::ROOK, color)]))
			generate_piece_moves<movgen::ROOK, gen_type>(piece_pos, pos, color, moves);
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::BISHOP, color)]))
			generate_piece_moves<movgen::BISHOP, gen_type>(piece_pos, pos, color, moves);
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::KNIGHT, color)]))
			generate_piece_moves<movgen::KNIGHT, gen_type>(piece_pos, pos, color, moves);
		movgen::generate_pawn_moves<color, gen_type>(pos, moves);
		break;
	case movgen::GenType::PROMOTIONS:
		movgen::generate_pawn_moves<color, gen_type>(pos, moves);
		break;
	case movgen::GenType::CASTLING:
		for(auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::KING, color)]))
			generate_piece_moves<movgen::KING, gen_type>(piece_pos, pos, color, moves);
		break;
	}

	if(pos.info != nullptr)
		delete pos.info;
	pos.info = new PositionInfo;

	get_checkers<col_them>(pos, pos.info);
	get_pinners<col_them>(pos, pos.info);

	get_attacked<movgen::WHITE>(pos, pos.info);
	get_attacked<movgen::BLACK>(pos, pos.info);
}

ALL_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::ALL_MOVES);
ALL_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::QUIETS);
ALL_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::CAPTURES);
ALL_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::PROMOTIONS);
ALL_MOVE_INSTANCE(movgen::WHITE, movgen::GenType::CASTLING);
ALL_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::ALL_MOVES);
ALL_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::QUIETS);
ALL_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::CAPTURES);
ALL_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::PROMOTIONS);
ALL_MOVE_INSTANCE(movgen::BLACK, movgen::GenType::CASTLING);

std::vector<movgen::Move> movgen::get_legal_moves(BoardPosition& pos, std::vector<Move>& generated)
{
	const Color c = pos.side_to_move;
	const unsigned int us = 8 * c;
	const bitboard pinned = pos.info->pin_board;
	const bpos ksq = bitb::pop_lsb(pos.pieces[B_KING + us]);

	const bitboard attacked = (pos.side_to_move == movgen::WHITE)
								  ? (pos.info->b_piece_attacks & pos.info->b_pawn_attacks & pos.info->b_king_attacks)
								  : (pos.info->w_piece_attacks & pos.info->w_pawn_attacks & pos.info->w_king_attacks);

	std::vector<movgen::Move> legal_moves;
	legal_moves.reserve(generated.size());

	// Only king moves are possible
	if(pos.info->checks_num >= 2)
	{
		for(Move move : generated)
		{
			if(move.from == ksq && move.get_type() != CASTLING)
			{
				if(!(attacked & bitb::sq_bitb(move.to)))
				{
					legal_moves.push_back(move);
				}
			}
		}
		return legal_moves;
	}
	// Only allow king moves and blockers
	if(pos.info->checks_num == 1)
	{
		for(Move move : generated)
		{
			if(bitb::sq_bitb(move.from) & pos.info->pin_board)
				continue;
			if((move.from == ksq && move.get_type() != CASTLING && _is_legal(pos, move)) ||
			   (move.from != ksq && bitb::sq_bitb(move.to) & pos.info->blockers))
			{
				legal_moves.push_back(move);
			}
		}
		return legal_moves;
	}

	for(Move move : generated)
	{
		// Check for legality only if one these 4 requirements is met
		if(!(bitb::sq_bitb(move.from) & pinned || move.from == ksq || move.get_type() == EN_PASSANT) || _is_legal(pos, move))
			legal_moves.push_back(move);
	}

	return legal_moves;
}

template <movgen::GenType gen_type>
void movgen::make_move(movgen::BoardPosition* pos, movgen::Move& move, std::vector<movgen::Move>* new_moves)
{
	const bitb::Direction down = pos->side_to_move == movgen::WHITE ? bitb::DOWN : bitb::UP;
	const movgen::CastlingRights castling = pos->side_to_move == movgen::WHITE ? movgen::WHITE_CASTLE : movgen::BLACK_CASTLE;

	const movgen::Color cur_color = pos->side_to_move;
	const uint16_t us = cur_color == movgen::BLACK ? 8 : 0;
	const uint16_t them = cur_color == movgen::BLACK ? 0 : 8;

	const movgen::Piece captured = move.get_type() == movgen::EN_PASSANT ? static_cast<movgen::Piece>(movgen::B_PAWN + them)
																		 : movgen::get_piece(*pos, move.to);

	pos->hash = new BoardHash(pos->hash);
	pos->hash->key ^= zobrist::side;

	// Flip the color&
	pos->side_to_move = static_cast<movgen::Color>(!pos->side_to_move);
	if(pos->side_to_move == movgen::WHITE)
		pos->fullmove++;
	pos->hash->ply++; // Reset later, if necessary
	pos->hash->en_passant = 0;

	switch(move.get_type())
	{
	case CAPTURE:
		pos->pieces[captured] &= ~(1ull << move.to);
		pos->squares[move.to] = movgen::NO_PIECE;
		pos->hash->ply = 0;

		pos->hash->key ^= zobrist::table[captured][move.to];
	// Fallthrough
	case REGULAR:
		_move_piece(pos, move.piece, move.from, move.to);
		pos->hash->key ^= zobrist::table[move.piece][move.from];
		pos->hash->key ^= zobrist::table[move.piece][move.to];
		break;
	case PROMOTION_CAPTURE:
		pos->pieces[pos->squares[move.to]] &= ~(1ull << move.to);
		pos->hash->key ^= zobrist::table[captured][move.to];
	// Fallthrough
	case PROMOTION: {
		movgen::PieceType prom = move.get_promoted();
		assert(2 <= prom && prom < 6);
		const movgen::Piece new_piece = get_piece_from_type(static_cast<movgen::PieceType>(prom), cur_color);

		pos->pieces[move.piece] &= ~(1ull << move.from);
		pos->pieces[new_piece] |= (1ull << move.to);

		pos->squares[move.from] = movgen::NO_PIECE;
		pos->squares[move.to] = new_piece;

		pos->hash->key ^= zobrist::table[move.piece][move.from];
		pos->hash->key ^= zobrist::table[new_piece][move.to];
		break;
	}
	case EN_PASSANT:
		pos->pieces[pos->squares[move.to + down]] ^= 1ull << (move.to + down);
		pos->squares[move.to + down] = movgen::NO_PIECE;
		_move_piece(pos, move.piece, move.from, move.to);

		pos->hash->key ^= zobrist::table[move.piece][move.from];
		pos->hash->key ^= zobrist::table[move.piece][move.to];
		pos->hash->key ^= zobrist::table[captured][move.to];
		break;
	case DOUBLE_MOVE:
		pos->hash->en_passant = move.to + down;
		_move_piece(pos, move.piece, move.from, move.to);

		pos->hash->key ^= zobrist::table[move.piece][move.from];
		pos->hash->key ^= zobrist::table[move.piece][move.to];
		break;
	case CASTLING: {
		const bitboard king_rank = bitb::sq_rank(move.from);
		const movgen::Piece rook_piece = get_piece_from_type(movgen::ROOK, cur_color);

		// Short castling
		if(((move.move_data >> 10) & 0x3) == 1)
		{
			_move_piece(pos, move.piece, move.from, move.to);
			_move_piece(pos, rook_piece, move.from - 3, move.from - 1);

			pos->hash->key ^= zobrist::table[rook_piece][move.from - 3];
			pos->hash->key ^= zobrist::table[rook_piece][move.from - 1];
		}
		else
		{
			_move_piece(pos, move.piece, move.from, move.to);
			_move_piece(pos, rook_piece, move.from + 4, move.from + 1);

			pos->hash->key ^= zobrist::table[rook_piece][move.from + 4];
			pos->hash->key ^= zobrist::table[rook_piece][move.from + 1];
		}

		pos->hash->castling_rights &= ~(cur_color == WHITE ? movgen::WHITE_CASTLE : BLACK_CASTLE);

		pos->hash->ply = 0;
		pos->hash->key ^= zobrist::table[move.piece][move.from];
		pos->hash->key ^= zobrist::table[move.piece][move.to];
		break;
	}
	}

	if(movgen::get_piece_type(move.piece) == movgen::PAWN)
		pos->hash->ply = 0;

	// Assign composite bitboards
	pos->pieces[BLACK_PIECES] = pos->pieces[B_KING] | pos->pieces[B_QUEEN] | pos->pieces[B_ROOK] | pos->pieces[B_BISHOP] |
								pos->pieces[B_KNIGHT] | pos->pieces[B_PAWN];
	pos->pieces[WHITE_PIECES] = pos->pieces[W_KING] | pos->pieces[W_QUEEN] | pos->pieces[W_ROOK] | pos->pieces[W_BISHOP] |
								pos->pieces[W_KNIGHT] | pos->pieces[W_PAWN];
	pos->pieces[ALL_PIECES] = pos->pieces[BLACK_PIECES] | pos->pieces[WHITE_PIECES];

	if(pos->hash->castling_rights & castling)
	{
		if(movgen::get_piece_type(move.piece) == movgen::KING)
		{
			pos->hash->ply = 0;
			pos->hash->key ^= zobrist::castling[castling];
			pos->hash->castling_rights ^= castling;
		}
		else if(movgen::get_piece_type(move.piece) == movgen::ROOK)
		{
			movgen::CastlingRights castling_change = movgen::NO_CASTLING;
			// h rank rook
			if(bitb::sq_bitb(move.from) & bitb::File[7])
				castling_change = static_cast<movgen::CastlingRights>(castling & movgen::SHORT);
			// a rank rook
			else if(bitb::sq_bitb(move.from) & bitb::File[0])
				castling_change = static_cast<movgen::CastlingRights>(castling & movgen::LONG);

			pos->hash->ply = 0;
			pos->hash->key ^= zobrist::castling[castling_change];
			pos->hash->castling_rights &= ~castling_change;
		}
	}

	// Check for 3 fold repetition rule
	if(pos->hash->ply > 2)
	{
		uint16_t reps = 0;
		movgen::BoardHash* hash_it = pos->hash;
		do
		{
			hash_it = hash_it->prev;
			reps += hash_it->key == pos->hash->key;
		} while(hash_it->ply > 0 && hash_it->prev != nullptr);

		assert(reps <= 3); // Should not be more than 3 in a normal game
		if(reps >= 3)
		{
			pos->repetiton_num = reps;
			return;
		}
	}

	if(new_moves != nullptr)
	{
		std::vector<movgen::Move> pseudo_legal;
		if(pos->side_to_move == movgen::WHITE)
			movgen::generate_all_moves<movgen::WHITE, gen_type>(*pos, &pseudo_legal);
		else
			movgen::generate_all_moves<movgen::BLACK, gen_type>(*pos, &pseudo_legal);

		*new_moves = movgen::get_legal_moves(*pos, pseudo_legal);
	}
}

MAKE_MOVE_INSTANCE(movgen::GenType::ALL_MOVES);
MAKE_MOVE_INSTANCE(movgen::GenType::QUIETS);
MAKE_MOVE_INSTANCE(movgen::GenType::CAPTURES);
MAKE_MOVE_INSTANCE(movgen::GenType::PROMOTIONS);
MAKE_MOVE_INSTANCE(movgen::GenType::CASTLING);

movgen::GameStatus movgen::check_game_state(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves)
{
	if(pos->repetiton_num >= 3)
		return movgen::DRAW;
	if(gen_moves.size() == 0)
	{
		if(pos->info->checks_num > 0)
			return pos->side_to_move == movgen::WHITE ? movgen::BLACK_WINS : movgen::WHITE_WINS;
		else
			return movgen::DRAW;
	}
	if(pos->hash->ply == 50)
		return movgen::DRAW;

	return movgen::GAME_CONTINUES;
}

void movgen::undo_move(movgen::BoardPosition* pos, movgen::Move& move)
{
	pos->side_to_move = static_cast<movgen::Color>(!pos->side_to_move);
	if(pos->side_to_move == movgen::BLACK)
		pos->fullmove--;

	const bitb::Direction down = pos->side_to_move == movgen::WHITE ? bitb::DOWN : bitb::UP;

	const movgen::Piece captured = move.get_captured();
	const movgen::Piece promoted = movgen::get_piece_from_type(move.get_promoted(), pos->side_to_move);

	_move_piece(pos, move.piece, move.to, move.from);

	switch(move.get_type())
	{
	case PROMOTION:
		pos->pieces[promoted] &= ~bitb::sq_bitb(move.to);
		break;
	case PROMOTION_CAPTURE:
		pos->pieces[promoted] &= ~bitb::sq_bitb(move.to);
		[[fallthrough]];
	case CAPTURE:
		pos->pieces[captured] |= bitb::sq_bitb(move.to);
		pos->squares[move.to] = captured;
		break;
	case EN_PASSANT:
		pos->pieces[captured] |= bitb::sq_bitb(move.to + down);
		pos->squares[move.to + down] = captured;
		break;
	case CASTLING: {
		movgen::Piece rook = get_piece_from_type(movgen::ROOK, pos->side_to_move);
		// Short castling
		if(((move.move_data >> 10) & 0x3) == 1)
			_move_piece(pos, rook, move.from - 1, move.to - 1);
		else
			_move_piece(pos, rook, move.from + 1, move.to + 2);
		break;
	}
	default:
		break;
	}

	movgen::BoardHash* cur = pos->hash;
	pos->hash = pos->hash->prev;
	pos->repetiton_num = 0;

	delete cur;

	// Assign composite bitboards
	pos->pieces[BLACK_PIECES] = pos->pieces[B_KING] | pos->pieces[B_QUEEN] | pos->pieces[B_ROOK] | pos->pieces[B_BISHOP] |
								pos->pieces[B_KNIGHT] | pos->pieces[B_PAWN];
	pos->pieces[WHITE_PIECES] = pos->pieces[W_KING] | pos->pieces[W_QUEEN] | pos->pieces[W_ROOK] | pos->pieces[W_BISHOP] |
								pos->pieces[W_KNIGHT] | pos->pieces[W_PAWN];
	pos->pieces[ALL_PIECES] = pos->pieces[BLACK_PIECES] | pos->pieces[WHITE_PIECES];
}

bool _is_legal(movgen::BoardPosition& pos, movgen::Move move)
{
	const movgen::Color c = pos.side_to_move;
	const bpos from = move.from;
	const bpos to = move.to;

	const bitboard attacked = (pos.side_to_move == movgen::WHITE)
								  ? (pos.info->b_piece_attacks | pos.info->b_pawn_attacks | pos.info->b_king_attacks)
								  : (pos.info->w_piece_attacks | pos.info->w_pawn_attacks | pos.info->w_king_attacks);

	if(move.get_type() == movgen::EN_PASSANT)
	{
		if(pos.info->pin_board & bitb::sq_bitb(move.from))
			return false;
		return !(pos.info->en_passant_pin);
	}
	else if(move.get_type() == movgen::CASTLING)
	{
		// Check that no square inbetween are under attack
		bitboard end_sq = from > to ? from - 2 : from + 2;
		return !(attacked & bitb::Between[from][end_sq]);
	}
	else if(get_piece_type(movgen::get_piece(pos, move.from)) == movgen::KING)
	{
		return !(attacked & bitb::sq_bitb(to));
	}

	// A pinned piece can move only to specified squares
	for(auto pin : pos.info->pins)
		if(pin.pinned == move.from)
			return bitb::sq_bitb(move.to) & pin.mask;
	return true;
}

void bitb_movearray(movgen::Piece piece,
					bpos starting_pos,
					bitboard move_board,
					bitboard them,
					movgen::BoardPosition& pos,
					std::vector<movgen::Move>* move_arr)
{
	bitboard captures = move_board & them;
	bitboard quiet = move_board ^ captures;

	for(auto pos_to : bitb::bitscan(quiet))
		move_arr->push_back(movgen::Move(piece, starting_pos, pos_to));
	for(auto pos_to : bitb::bitscan(captures))
		move_arr->push_back(movgen::Move(piece, starting_pos, pos_to, movgen::get_piece(pos, pos_to)));
}

template <movgen::Color c, bitb::Direction d>
inline void _make_promotions(std::vector<movgen::Move>* move_arr, bpos to, unsigned char capture)
{
	constexpr movgen::Piece piece = (c == movgen::WHITE ? movgen::W_PAWN : movgen::B_PAWN);

	move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::QUEEN, false, false, 0));
	move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::ROOK, false, false, 0));
	move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::BISHOP, false, false, 0));
	move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::KNIGHT, false, false, 0));
}

void _move_piece(movgen::BoardPosition* pos, movgen::Piece piece, bpos from, bpos to)
{
	pos->pieces[piece] &= ~bitb::sq_bitb(from);
	pos->pieces[piece] |= bitb::sq_bitb(to);

	pos->squares[from] = movgen::NO_PIECE;
	pos->squares[to] = piece;
}
