#include "../headers/search.h"
#include "../headers/eval.h"
#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <ctime>
#include <vector>

#define fuzzy_equal(val1, val2) std::abs(val1 - val2) < 0.01

float minmax_eval(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth = 5)
{
	_transposition_table.clear();
	_SearchType type = pos->side_to_move == movgen::WHITE ? _SearchType::MAX : _SearchType::MIN;

	if(depth == 0)
	{
		if(type == _SearchType::MAX)
			return eval(*pos);
		else
			return -eval(*pos);
	}
	return __minmax<_SearchType::MIN>(pos, gen_moves, depth, -INFINITY, INFINITY);
}

std::tuple<float, movgen::Move> minmax_best(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth)
{
	assert(depth > 0);
	_transposition_table.clear();

	_SearchType type = pos->side_to_move == movgen::WHITE ? _SearchType::MAX : _SearchType::MIN;

	float alpha = -INFINITY, beta = INFINITY;
	float score;
	float best_value = (type == _SearchType::MAX ? -INFINITY : INFINITY);
	uint16_t it = 0;

// If not debug, choose randomly among the moves with the best eval, else choose the first move
#ifdef NDEBUG
	std::vector<uint16_t> best_move_index;
#else
	uint16_t best_move_index;
#endif

	std::vector<movgen::Move> new_moves;
	for(auto& move : gen_moves)
	{
		new_moves.clear();

		movgen::make_move(pos, move, &new_moves);
		// Recursion call here
		if(type == _SearchType::MAX)
			score = __minmax<_SearchType::MIN>(pos, new_moves, depth - 1, alpha, beta);
		else
			score = __minmax<_SearchType::MAX>(pos, new_moves, depth - 1, alpha, beta);
		movgen::undo_move(pos, move);

		if(type == _SearchType::MAX)
		{
			if(score > best_value)
			{
				best_value = score;

#ifdef NDEBUG
				best_move_index.clear();
				best_move_index.push_back(it);
#else
				best_move_index = it;
#endif

				if(score > alpha)
					alpha = score; // alpha acts like max in MiniMax
			}
#ifdef NDEBUG
			else if(fuzzy_equal(score, best_value))
				best_move_index.push_back(it);
#endif
			if(score >= beta)
				break; // fail soft beta-cutoff
		}
		else
		{
			if(score < best_value)
			{
				best_value = score;

#ifdef NDEBUG
				best_move_index.clear();
				best_move_index.push_back(it);
#else
				best_move_index = it;
#endif

				if(score < beta)
					beta = score; // beta acts like min in MiniMax
			}
#ifdef NDEBUG
			else if(fuzzy_equal(score, best_value))
				best_move_index.push_back(it);
#endif
			if(score <= alpha)
				break; // fail soft alpha-cutoffs
		}
		++it;
	}

#ifdef NDEBUG
	uint16_t final_index = uint16_t(rand() % best_move_index.size());
	return std::make_tuple(best_value, gen_moves[best_move_index[final_index]]);
#else
	return std::make_tuple(best_value, gen_moves[best_move_index]);
#endif
}

std::vector<std::tuple<float, movgen::Move>>
minmax_all(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth)
{
	assert(depth > 0);
	_transposition_table.clear();

	std::vector<std::tuple<float, movgen::Move>> move_eval;
	move_eval.reserve(gen_moves.size());

	std::vector<movgen::Move> new_moves;
	// Trigger the search
	for(auto& move : gen_moves)
	{
		new_moves.clear();
		float score;

		movgen::make_move(pos, move, &new_moves);
		if(!eval_if_game_ended(pos, new_moves, &score))
			score = minmax_eval(pos, new_moves, depth - 1);
		movgen::undo_move(pos, move);

		move_eval.push_back(std::make_tuple(score, move));
	}

	return move_eval;
}

template <_SearchType type>
float __minmax(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, uint16_t depth, float alpha, float beta)
{
	// Position has been reached return the cashed score
	if(auto row = _transposition_table.find(pos->hash->key); row != _transposition_table.end())
		return row->second.score;

	float score;
	if(eval_if_game_ended(pos, gen_moves, &score))
		return score;
	// Search remaining captures and only then return the score
	if(depth == 0)
	{
		if(type == _SearchType::MAX)
			return _minmax_captures<_SearchType::MIN>(pos, alpha, beta);
		else
			return _minmax_captures<_SearchType::MAX>(pos, alpha, beta);
	}

	float best_value = (type == _SearchType::MAX ? -INFINITY : INFINITY);

	std::vector<movgen::Move> new_moves;
	for(auto& move : gen_moves)
	{
		new_moves.clear();
		movgen::make_move(pos, move, &new_moves);

		// Recursion call here
		if(type == _SearchType::MAX)
			score = __minmax<_SearchType::MIN>(pos, new_moves, depth - 1, alpha, beta);
		else
			score = __minmax<_SearchType::MAX>(pos, new_moves, depth - 1, alpha, beta);

		//We have fully searched the position, cache the score
		_transposition_table.emplace(std::make_pair(pos->hash->key, _TranspositionTableRow({score})));
		movgen::undo_move(pos, move);

		if(type == _SearchType::MAX)
		{
			if(score > best_value)
			{
				if(score > alpha)
					alpha = score; // alpha acts like max in MiniMax
				best_value = alpha;
			}
			if(score >= beta)
				return score; // fail soft beta-cutoff
		}
		else
		{
			if(score < best_value)
			{
				if(score < beta)
					beta = score; // beta acts like min in MiniMax
				best_value = score;
			}
			if(score <= alpha)
				return score; // fail soft alpha-cutoffs
		}
	}
	return best_value;
}

// Implement Quiescence Search
// Search only captures, for indefinite depth
template <_SearchType type>
float _minmax_captures(movgen::BoardPosition* pos, float alpha, float beta)
{
	float score;
	float best_value = (type == _SearchType::MAX ? -INFINITY : INFINITY);

	if(type == _SearchType::MAX)
		score = eval(*pos);
	else
		score = -eval(*pos);

	if(type == _SearchType::MAX)
	{
		if(score > best_value)
		{
			if(score > alpha)
				alpha = score; // alpha acts like max in MiniMax
			best_value = alpha;
		}
		if(score >= beta)
			return score; // fail soft beta-cutoff
	}
	else
	{
		if(score < best_value)
		{
			if(score < beta)
				beta = score; // beta acts like min in MiniMax
			best_value = score;
		}
		if(score <= alpha)
			return score; // fail soft alpha-cutoffs
	}

	std::vector<movgen::Move> pseudo_legal, new_moves;
	if(type == _SearchType::MAX)
		movgen::generate_all_moves<movgen::BLACK, movgen::GenType::CAPTURES>(*pos, &pseudo_legal);
	else
		movgen::generate_all_moves<movgen::WHITE, movgen::GenType::CAPTURES>(*pos, &pseudo_legal);
	new_moves = movgen::get_legal_moves(*pos, pseudo_legal);

	for(auto& move : new_moves)
	{
		movgen::make_move(pos, move, nullptr);

		// Recursion call here
		if(type == _SearchType::MAX)
			score = _minmax_captures<_SearchType::MIN>(pos, alpha, beta);
		else
			score = _minmax_captures<_SearchType::MAX>(pos, alpha, beta);

		//We have fully searched the position, cache the score
		_transposition_table.emplace(std::make_pair(pos->hash->key, _TranspositionTableRow({score})));
		movgen::undo_move(pos, move);

		if(type == _SearchType::MAX)
		{
			if(score > best_value)
			{
				if(score > alpha)
					alpha = score; // alpha acts like max in MiniMax
				best_value = alpha;
			}
			if(score >= beta)
				return score; // fail soft beta-cutoff
		}
		else
		{
			if(score < best_value)
			{
				if(score < beta)
					beta = score; // beta acts like min in MiniMax
				best_value = score;
			}
			if(score <= alpha)
				return score; // fail soft alpha-cutoffs
		}
	}
	return best_value;
}

bool eval_if_game_ended(movgen::GameStatus status, float* eval)
{
	switch(status)
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

bool eval_if_game_ended(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves, float* eval)
{
	return eval_if_game_ended(movgen::check_game_state(pos, gen_moves), eval);
}
