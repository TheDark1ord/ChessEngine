#include "../headers/commands.h"
#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include <cstddef>
#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

void save_position(std::vector<std::string> args)
{
	if(args.empty())
	{
		std::cout << "Please provide position string as argument";
		return;
	}

	size_t it = 0;
	std::string fen;
	if(args[0] == "startpos")
	{
		fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
	}
	else if(args[0] == "fen")
	{ // Fen string contains spaces so we have to concatenate these strings
		for(it = 1; it < args.size() && args[it] != "moves"; it++)
			fen.append(args[it]);
	}
	else
	{
		std::cout << "Please use keywords 'startpos' or 'fen' to specify position";
		return;
	}

	try
	{
		_saved_pos = movgen::board_from_fen(args[0]);
	}
	catch(std::runtime_error e)
	{
		std::cout << "Failed to construct position. Aborting\n\n";
		std::cout << e.what() << std::endl;
		return;
	}
	_saved_pos_is_null = false;

	if(args.size() > it && args[it] == "moves")
	{
		try
		{
			_make_moves(args.begin() + it + 1, args.end());
		}
		catch(std::runtime_error e)
		{
			_saved_pos_is_null = true;
			printf("Error: %s\n", e.what());
		}
	}
}


void _make_moves(std::vector<std::string>::iterator begin, std::vector<std::string>::iterator end)
{
	auto iterator = begin;

	auto cur_moves = _generate_moves();
	while(iterator != end)
	{
		bool made_move = false;

		std::string move_string, prom_string;
		//In case of promotion also provide a piece to promote to
		if(iterator->length() > 4)
		{
			move_string = iterator->substr(0, iterator->find(':'));

			prom_string = iterator->substr(iterator->find(':'));
			// Convert to upper case for comparison
			std::transform(prom_string.begin(), prom_string.end(), prom_string.begin(), ::toupper);
		}
		else
			move_string = iterator->substr(0, iterator->find(':'));

		std::vector<movgen::Move> new_moves = cur_moves;
		for(auto& move : new_moves)
		{
			if((std::string(_squares[move.from]) + (std::string(_squares[move.to]))) == move_string)
			{
				if(prom_string != "" && _piece_types[move.get_promoted()] != prom_string)
					continue;

				movgen::make_move(&_saved_pos, move, &new_moves);
				made_move = true;
				break;
			}
		}

		if(!made_move)
		{
			printf("Could not find move \"%s\", position not set\n", iterator->c_str());
			throw std::runtime_error("Specified move not found");
		}
		iterator++;
	}

	_gen_moves = cur_moves;
}

std::vector<movgen::Move> _generate_moves()
{
	if(_saved_pos_is_null)
		throw std::runtime_error("Please initialise a position");

	std::vector<movgen::Move> pseudo_moves;
	std::vector<movgen::Move> legal_moves;

	_saved_pos.side_to_move == movgen::WHITE
		? movgen::generate_all_moves<movgen::WHITE>(_saved_pos, &pseudo_moves)
		: movgen::generate_all_moves<movgen::BLACK>(_saved_pos, &pseudo_moves);
	movgen::get_legal_moves(_saved_pos, pseudo_moves, &legal_moves);

	return legal_moves;
}

void start_search(std::vector<std::string> args) { }
void start_perft(std::vector<std::string> args) { }