#include "MovgenTypes.h"
#include <climits>
#include <cmath>
#include <cstdint>

// Return only evaluation
float minmax_eval(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth);
// Return best move adn it's eval
std::tuple<float, movgen::Move>
minmax_best(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth);
// Return all moves and their evals
std::vector<std::tuple<float, movgen::Move>>
minmax_all(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth);

enum class _SearchType
{
	MAX,
	MIN
};
template <_SearchType type>
static float
__minmax(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth, float alpha, float beta);

// Return eval, if the game ended
static bool eval_if_game_ended(movgen::GameStatus status, float* eval);
static bool eval_if_game_ended(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, float* eval);