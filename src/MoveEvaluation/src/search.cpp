#include "../headers/search.h"
#include "../headers/eval.h"
#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include <climits>
#include <cmath>
#include <cstdint>
#include <vector>

float minmax_search(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth = 5)
{
	return __minimax<_SearchType::MAX>(pos, gen_moves, depth, -INFINITY, INFINITY);
}

template <_SearchType type>
float __minimax(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth, float alpha, float beta)
{
	float score;
	if (eval_if_game_ended(pos, gen_moves, &score))
		return score;
	if (depth == 0)
	{
		if (type == _SearchType::MAX)
			return eval(*pos);
		else
			return -eval(*pos);
	}

	float best_value = (type == _SearchType::MAX ? -INFINITY : INFINITY);

	std::vector<movgen::Move> new_moves;
	for (auto &move : gen_moves)
	{
		new_moves.clear();
		movgen::make_move(pos, move, &new_moves);

		// Recursion call here
		if (type == _SearchType::MAX)
			score = __minimax<_SearchType::MIN>(pos, new_moves, depth - 1, alpha, beta);
		else
			score = __minimax<_SearchType::MAX>(pos, new_moves, depth - 1, alpha, beta);
		movgen::undo_move(pos, move);

		if (type == _SearchType::MAX)
		{
			if (score > best_value)
			{
				best_value = alpha;
				if (score > alpha)
					alpha = score; // alpha acts like max in MiniMax
			}
			if (score >= beta)
				return score; // fail soft beta-cutoff
		}
		else
		{
			if (score < best_value)
			{
				best_value = score;
				if (score < beta)
					beta = score; // beta acts like min in MiniMax
			}
			if (score <= alpha)
				return score; // fail soft alpha-cutoffs
		}
	}
	return best_value;
}

movgen::Move get_best_move(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, uint16_t depth = 5)
{
	std::vector<float> move_eval;
	move_eval.resize(gen_moves.size());
	uint16_t it = 0;

	std::vector<movgen::Move> new_moves;
	// Trigger the search
	for (auto &move : gen_moves)
	{
		new_moves.clear();

		movgen::make_move(pos, move, &new_moves);
		if (!eval_if_game_ended(pos, new_moves, &move_eval[it]))
			move_eval[it] = minmax_search(pos, new_moves, depth - 1);
		movgen::undo_move(pos, move);

		it++;
	}

	// Get max evaluation
	float max_eval = move_eval[0];
	int max_index = 0;
	for (int i = 1; i < move_eval.size(); i++)
	{
		if (move_eval[i] > max_eval)
		{
			max_eval = move_eval[i];
			max_index = i;
		}
	}
	return gen_moves[max_index];
}

bool eval_if_game_ended(movgen::GameStatus status, float *eval)
{
	switch (status)
	{
	case movgen::GAME_CONTINUES:
		return false;
	case movgen::DRAW:
		*eval = 0.0;
		return true;
	case movgen::WHITE_WINS:
		*eval = INFINITY;
		return true;
	case movgen::BLACK_WINS:
		*eval = -INFINITY;
		return true;
	}
}

bool eval_if_game_ended(movgen::BoardPosition *pos, std::vector<movgen::Move> &gen_moves, float *eval)
{
	return eval_if_game_ended(movgen::check_game_state(pos, gen_moves), eval);
}
