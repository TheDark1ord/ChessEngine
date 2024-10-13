#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include "eval.h"
#include "search.h"

#include <cstdint>
#include <cstdlib>
#include <thread>
#include <vector>

#define MAX_DEPTH 6

const std::string fen_strings[]{
	"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", // Initial position
	"r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", // Kiwipete by Peter McKenzie
	"8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -", // pos 3
	"r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", // pos 4
	"r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", // Mirrored pos 4
	"rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", // pos 5
};

template <_SearchType type>
float _minmax(movgen::BoardPosition* pos,
					 std::vector<movgen::Move>& gen_moves,
					 uint16_t depth,
					 float alpha,
					 float beta,
					 size_t* nodes_reached);

int main(int argc, char* argv[])
{
	std::thread init_thread1(movgen::init);
	std::thread init_thread2(bitb::init);

	init_thread1.detach();
	init_thread2.join();

	uint16_t test_index = std::atoi(argv[1]);
	movgen::BoardPosition pos = movgen::board_from_fen(fen_strings[test_index]);
	std::vector<movgen::Move> pseudo_moves;
	std::vector<movgen::Move> legal_moves;

	pos.side_to_move == movgen::WHITE ? movgen::generate_all_moves<movgen::WHITE>(pos, &pseudo_moves)
									  : movgen::generate_all_moves<movgen::BLACK>(pos, &pseudo_moves);
	movgen::get_legal_moves(pos, pseudo_moves, &legal_moves);
	size_t nodes_reached = 0;

	for(int depth = 0; depth <= MAX_DEPTH; depth++)
	{
		_minmax<_SearchType::MAX>(&pos, legal_moves, (uint16_t)depth, -INFINITY, INFINITY, &nodes_reached);
		printf("Depth: %i, nodes searched: %lli\n", depth, nodes_reached);
		nodes_reached = 0;
	}

	return 0;
}

template <_SearchType type>
float _minmax(movgen::BoardPosition* pos,
			  std::vector<movgen::Move>& gen_moves,
			  uint16_t depth,
			  float alpha,
			  float beta,
			  size_t* nodes_reached)
{
	(*nodes_reached)++;

	float score;
	if(eval_if_game_ended(pos, gen_moves, &score))
		return score;
	if(depth == 0)
	{
		if(type == _SearchType::MAX)
			return eval(*pos);
		else
			return -eval(*pos);
	}

	float best_value = (type == _SearchType::MAX ? -INFINITY : INFINITY);

	std::vector<movgen::Move> new_moves;
	for(auto& move : gen_moves)
	{
		new_moves.clear();
		movgen::make_move(pos, move, &new_moves);

		// Recursion call here
		if(type == _SearchType::MAX)
			score = _minmax<_SearchType::MIN>(pos, new_moves, depth - 1, alpha, beta, nodes_reached);
		else
			score = _minmax<_SearchType::MAX>(pos, new_moves, depth - 1, alpha, beta, nodes_reached);
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