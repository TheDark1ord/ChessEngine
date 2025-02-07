#ifndef TYPES_H
#define TYPES_H

#include "Bitboard.h"
#include <cstdint>
#include <regex>
#include <string>
#include <vector>

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

enum class GenType
{
	ALL_MOVES,
	QUIETS,
	CAPTURES,
	PROMOTIONS,
	CASTLING
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

	bitboard w_piece_attacks = 0;
	bitboard w_pawn_attacks = 0;
	bitboard w_king_attacks = 0;

	bitboard b_piece_attacks = 0;
	bitboard b_pawn_attacks = 0;
	bitboard b_king_attacks = 0;
};

class BoardPosition;

class BoardHash
{
public:
	BoardHash(BoardPosition& pos);
	BoardHash(BoardHash*& prev);

	// Data for undoing a move
	int castling_rights;
	bpos en_passant;
	uint32_t ply = 0; // Halfmove

	size_t key;
	BoardHash* prev;

	bool operator==(const BoardHash& other) const;
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
	unsigned int repetiton_num; // For use in check_game_state only. Resets to 0 in undo move

	movgen::BoardHash* hash = new movgen::BoardHash(*this);
	PositionInfo* info = nullptr;

	void print();
};

inline const char* fen_regex_string = "\\s*^(((?:[rnbqkpRNBQKP1-8]+\\/){7})[rnbqkpRNBQKP1-8]+)"
									  "\\s*([b|w])\\s*([K|Q|k|q]{1,4}|-)\\s*(-|[a-h][1-8])\\s*(\\d+\\s\\d+){0,1}\\s*$";

BoardPosition board_from_fen(std::string fen);
std::string board_to_fen(BoardPosition& pos);

class Move
{
public:
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
	//      15 -- unknown piece
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

	Move();
	Move(Piece piece, bpos from, bpos to);
	Move(Piece piece,
		 bpos from,
		 bpos to,
		 unsigned char capture,
		 unsigned char promotion = 0,
		 bool double_move = 0,
		 bool en_passant = 0,
		 unsigned char castling = 0);
	Move(const Move& other);

	MoveType get_type() const;
	Piece get_captured() const;
	PieceType get_promoted() const;

	static constexpr const char* const squares[]{
		"h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1", "h2", "g2", "f2", "e2", "d2",
		"c2", "b2", "a2", "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3", "h4", "g4",
		"f4", "e4", "d4", "c4", "b4", "a4", "h5", "g5", "f5", "e5", "d5", "c5", "b5",
		"a5", "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6", "h7", "g7", "f7", "e7",
		"d7", "c7", "b7", "a7", "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
	};
	static constexpr char const piece_str[]{
		0, 'k', 'q', 'r', 'b', 'n', 'p'
	};
	operator std::string() const;

	bool is_null_instance = false;
	static Move return_null();
};

Piece get_piece(BoardPosition& b_pos, bpos pos);

constexpr movgen::PieceType get_piece_type(movgen::Piece piece)
{
	switch(piece)
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

PieceType get_piece_type(BoardPosition& b_pos, bpos pos);

movgen::Color get_piece_color(movgen::Piece piece);

constexpr movgen::Piece get_piece_from_type(movgen::PieceType type, movgen::Color c)
{
	switch(type)
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
	size_t operator()(movgen::BoardHash const& p) const noexcept;
};

// Define a hash function for BoardPosition
template <>
struct std::hash<movgen::BoardPosition>
{
	size_t operator()(movgen::BoardPosition const& p) const noexcept;
};

#endif
