#include "../headers/search.h"
#include "../headers/eval.h"
#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include <climits>
#include <cmath>
#include <cstdint>
#include <vector>

float minmax_search(movgen::BoardPosition* pos, std::vector<movgen::Move>& gen_moves)
{
	_SearchArgs args(pos, 5);
	return __search<_SearchType::MAX>(gen_moves, args);
}

template <_SearchType type>
float __search(std::vector<movgen::Move>& gen_moves, _SearchArgs& args)
{
	if(args.depth == 0)
	{
		if(type == _SearchType::MAX)
			return eval(*args.pos);
		else
			return -eval(*args.pos);
	}

	float best_value = (type == _SearchType::MAX ? INT_MIN : INT_MAX);
	//Update depth
	args.depth--;

	std::vector<movgen::Move> new_moves;
	float score;
	for(auto& move : gen_moves)
	{
		new_moves.clear();

		auto result = movgen::make_move(args.pos, move, &new_moves);
		// Recursion call here
		if(!check_game_state(result, &score))
		{
			if(type == _SearchType::MAX)
				score = __search<_SearchType::MIN>(new_moves, args);
			else
				score = __search<_SearchType::MAX>(new_moves, args);

			// Revert depth
			args.depth++;
		}
		movgen::undo_move(args.pos, move);

		if(type == _SearchType::MAX)
		{
			if(score > best_value)
			{
				best_value = args.alpha;
				if(score > args.alpha)
					args.alpha = score; // alpha acts like max in MiniMax
			}
			if(score >= args.beta)
				return score; // fail soft beta-cutoff
		}
		else
		{
			if(score < best_value)
			{
				best_value = score;
				if(score < args.beta)
					args.beta = score; // beta acts like min in MiniMax
			}
			if(score <= args.alpha)
				return score; // fail soft alpha-cutoffs
		}
	}
	return best_value;
}

movgen::Move get_best_move(movgen::BoardPosition& pos,
						   std::vector<movgen::Move>* gen_moves = nullptr)
{
	//Generate moves for root position
	if(gen_moves == nullptr)
	{
		std::vector<movgen::Move> pseudo_moves;
		gen_moves = new std::vector<movgen::Move>;

		pos.side_to_move == movgen::WHITE
			? movgen::generate_all_moves<movgen::WHITE>(pos, &pseudo_moves)
			: movgen::generate_all_moves<movgen::BLACK>(pos, &pseudo_moves);
		movgen::get_legal_moves(pos, pseudo_moves, gen_moves);
	}
	if (gen_moves->empty())
		return movgen::Move(movgen::NO_PIECE, 0, 0);

	std::vector<float> move_eval;
	move_eval.resize(gen_moves->size());
	uint16_t it = 0;

	std::vector<movgen::Move> new_moves;
	//Trigger the search
	for(auto& move : *gen_moves)
	{
		new_moves.clear();
		movgen::GameStatus result;

		result = movgen::make_move(&pos, move, &new_moves);
		if(!check_game_state(result, &move_eval[it]))
			move_eval[it] = minmax_search(&pos, new_moves);
		movgen::undo_move(&pos, move);

		it++;
	}

	//Get max evaluation
	float max_eval = move_eval[0];
	int max_index = 0;
	for (int i = 1; i < move_eval.size(); i++)
	{
		if (i > max_eval)
		{
			max_eval = move_eval[i];
			max_index = i;
		}
	}
	return (*gen_moves)[max_index];
}

bool check_game_state(movgen::GameStatus status, float* eval)
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
