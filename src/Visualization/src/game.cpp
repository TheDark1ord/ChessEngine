#include "../headers/game.h"
#include <thread>

Chess::Chess(sf::Vector2u window_size, GameMode mode)
	: Chess(window_size, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", mode)
{ }

Chess::Chess(sf::Vector2u window_size, std::string fen, GameMode mode)
	: fen_string(fen.c_str()),
	  window(sf::VideoMode(window_size.x, window_size.y), "Chess"),
	  board(new Board(window_size)),
	  cur_moves(new std::vector<movgen::Move>),
	  mode(mode),
{
	game_icon.loadFromFile("../data/icon.png");
	window.setIcon(game_icon.getSize().x, game_icon.getSize().y, game_icon.getPixelsPtr());
	window.setFramerateLimit(60.0f);

	position = movgen::board_from_fen(fen_string);

	std::vector<movgen::Move>* all_moves = new std::vector<movgen::Move>;
	std::vector<movgen::Move>** legal_moves = &this->cur_moves;

	auto move_generation = [&all_moves, &legal_moves](movgen::BoardPosition position) {
		// Wait for all modules to initialize
		while(!movgen::initialized || !bitb::initialized || !movgen::initialized_magics)
		{
			std::this_thread::sleep_for(5ms);
		}

		position.side_to_move == movgen::WHITE
			? movgen::generate_all_moves<movgen::WHITE, movgen::GenType::ALL_MOVES>(position, all_moves)
			: movgen::generate_all_moves<movgen::BLACK, movgen::GenType::ALL_MOVES>(position, all_moves);
		**legal_moves = movgen::get_legal_moves(position, *all_moves);
	};
	std::thread movegen_thread(move_generation, std::ref(this->position));

	display();
	movegen_thread.join();
}

void Chess::loop()
{
	while(window.isOpen())
	{
		sf::Event event;
		while(window.pollEvent(event))
		{
			handle_event(event);
		}

		display();
	}
}

void Chess::handle_event(sf::Event ev)
{
	switch(ev.type)
	{
	case sf::Event::Closed:
		window.close();
		break;

	case sf::Event::KeyReleased:
		switch(ev.key.code)
		{
		case sf::Keyboard::F:
			board->flip_board();
			break;
		case sf::Keyboard::Left:
			this->reset_move();
			break;
		default:
			break;
		}
		break;

	case sf::Event::MouseButtonPressed:
		switch(ev.mouseButton.button)
		{
		case sf::Mouse::Left:
			this->handle_left_button_press();
			break;
		case sf::Mouse::Right:
			board->deselect_square();
			selected_piece_moves.clear();
			break;

		default:
			break;
		}
		break;

	case sf::Event::Resized: {
		this->handle_resized_event(ev.size);
		break;
	}

	default:
		break;
	}
}

void Chess::reset_move()
{
	if(!prev_moves.empty())
	{
		movgen::undo_move(&position, prev_moves.top());
		position.side_to_move == movgen::WHITE
			? movgen::generate_all_moves<movgen::WHITE, movgen::GenType::ALL_MOVES>(position, cur_moves)
			: movgen::generate_all_moves<movgen::BLACK, movgen::GenType::ALL_MOVES>(position, cur_moves);
		*cur_moves = movgen::get_legal_moves(position, *cur_moves);

		prev_moves.pop();
	}
}

void Chess::handle_left_button_press()
{
	sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

	if(!board->within_bounds(mouse_pos.x, mouse_pos.y))
		return;

	if(board->get_selected_square() != -1 && !selected_piece_moves.empty())
	{
		board->select_square(mouse_pos);
		move_piece();
	}
	else
		board->select_square(mouse_pos);

	update_piece_moves_highlight();
}

void Chess::move_piece()
{
	for(auto move : selected_piece_moves)
	{
		if(move.to == board->get_selected_square())
		{
			movgen::make_move<movgen::GenType::ALL_MOVES>(&position, move, cur_moves);
			auto game_status = movgen::check_game_state(&position, *cur_moves);

			prev_moves.push(move);
			board->deselect_square();

			switch(game_status)
			{
			case movgen::GAME_CONTINUES:
				break;
			case movgen::DRAW:
				printf("Draw\n");
				break;
			case movgen::BLACK_WINS:
				printf("Black wins\n");
				break;
			case movgen::WHITE_WINS:
				printf("White wins\n");
				break;
			}
		}
	}
}

void Chess::handle_resized_event(sf::Event::SizeEvent size)
{
	window.setView(sf::View(sf::FloatRect(0, 0, size.width, size.height)));

	int selected_square = board->get_selected_square();
	bool is_flipped = board->is_flipped();

	delete board;
	board = new Board(window.getSize(), is_flipped);

	if(selected_square != -1)
		board->select_square(selected_square % 8, selected_square / 8);
}

void Chess::update_piece_moves_highlight()
{
	int selected = board->get_selected_square();
	selected_piece_moves.clear();
	if(selected != -1)
	{
		// Check if there is a piece on that square
		if(position.pieces[movgen::ALL_PIECES] & (1ull << selected))
		{
			for(auto& move : *cur_moves)
			{
				if(move.from == selected)
				{
					selected_piece_moves.push_back(move);
				}
			}
		}
	}
}

void Chess::display()
{
	window.clear(sf::Color(255, 255, 240));

	board->draw_board(&window, &position);

	if(!selected_piece_moves.empty())
		board->draw_piece_moves(&window, selected_piece_moves);

	window.display();
}

EngineChildProcess::EngineChildProcess()
{

}

std::string EngineChildProcess::engine_search(std::string fen)
{

}

void EngineChildProcess::create_child_process()
{

}
