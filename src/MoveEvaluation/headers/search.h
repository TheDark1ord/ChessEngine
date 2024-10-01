#include "MovgenTypes.h"
#include <climits>
#include <cmath>
#include <cstdint>

movgen::Move get_best_move(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth);
// A wrapper around search function, that auto allocates _SearchArgs
float minmax_search(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth);

// Save all variables, that are persistent across recursive calls to
// allocate only one pointer per funtion call
struct _SearchArgs
{
	_SearchArgs(movgen::BoardPosition *pos, uint16_t max_depth)
		: pos(pos), depth(max_depth)
	{
	}

	movgen::BoardPosition *pos;
	uint16_t depth;

	int alpha = INT_MIN;
	int beta = INT_MAX;
};
enum class _SearchType
{
	MAX,
	MIN
};

template <_SearchType type>
static float __minimax(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth, float alpha, float beta);

// Return eval, if the game ended
static bool eval_if_game_ended(movgen::GameStatus status, float *eval);
static bool eval_if_game_ended(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, float *eval);