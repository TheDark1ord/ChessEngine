#include "../headers/game.h"
#include <thread>
#include <format>
#include <filesystem>

Chess::Chess(sf::Vector2u window_size)
    : Chess(window_size, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")
{
}

Chess::Chess(sf::Vector2u window_size, std::string fen)
    : fen_string(fen.c_str()), window(sf::VideoMode(window_size.x, window_size.y), "Chess")
{
    window.setFramerateLimit(60.0f);

	if(std::filesystem::exists("../data"))
		this->data_dir = "../data";
	else
		this->data_dir = "data";

	this->board = new Board(window_size, this->data_dir);

    game_icon.loadFromFile(std::format("{}/icon.png", data_dir));
    window.setIcon(game_icon.getSize().x, game_icon.getSize().y, game_icon.getPixelsPtr());

    position = movgen::board_from_fen(fen_string);

    std::vector<movgen::Move> *all_moves;
    std::vector<movgen::Move> **legal = &this->cur_moves;

    auto move_generation = [&all_moves, &legal](movgen::BoardPosition &position)
    {
        // Wait for all files to initialize
        while (!movgen::initialized || !bitb::initialized || !movgen::initialized_magics)
            std::this_thread::sleep_for(10ms);

        all_moves = position.side_to_move == movgen::WHITE ?
            movgen::generate_all_moves<movgen::WHITE>(position) :
            movgen::generate_all_moves<movgen::BLACK>(position);
        *legal = movgen::get_legal_moves(position, *all_moves);
    };
    std::thread movegen_th(move_generation, std::ref(this->position));

    display();
    movegen_th.join();
}

void Chess::loop()
{
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            handle_event(event);
        }

        display();
    }
}

void Chess::handle_event(sf::Event ev)
{
    switch (ev.type)
    {
    case sf::Event::Closed:
        window.close();
        break;

    case sf::Event::KeyReleased:
        switch (ev.key.code)
        {
        case sf::Keyboard::F:
            board->flip_board();
            break;
        case sf::Keyboard::Left:
            undo_move();
            get_sel_moves();
            break;
        default:
            break;
        }
        break;

    case sf::Event::MouseButtonPressed:
        switch (ev.mouseButton.button)
        {
        case sf::Mouse::Left:
            make_move();
            get_sel_moves();
            break;
        case sf::Mouse::Right:
            board->deselect_square();
            selected_piece_moves.clear();
            break;

        default:
            break;
        }
        break;

    case sf::Event::Resized:
    {
        window.setView(sf::View(sf::FloatRect(0, 0, ev.size.width, ev.size.height)));

        int selected_square = board->get_selected_square();
        bool is_flipped = board->is_flipped();

        delete board;
        board = new Board(window.getSize(),this->data_dir, is_flipped);

        if (selected_square != -1)
            board->select_square(selected_square % 8, selected_square / 8);

        break;
    }

    default:
        break;
    }
}

void Chess::display()
{
    window.clear(sf::Color(255, 255, 240));

    board->draw_board(&window, &position);

    if (!selected_piece_moves.empty())
        board->draw_piece_moves(&window, selected_piece_moves);
    if (position.info != nullptr && position.info->checks_num > 0)
        board->draw_check(&window, bitb::pop_lsb(position.pieces[movgen::get_piece_from_type(movgen::KING, position.side_to_move)]));

    window.display();
}

void Chess::make_move()
{
    sf::Vector2i mouse_pos = sf::Mouse::getPosition(window);

    if (!board->within_bounds(mouse_pos.x, mouse_pos.y))
        return;

    if (board->get_selected_square() != -1 && !selected_piece_moves.empty())
    {
        board->select_square(mouse_pos);
        for (auto move : selected_piece_moves)
        {
            if (move.to == board->get_selected_square())
            {
                sf::Vector2<uint16_t> prev_highlight;
                if (prev_moves.empty())
                    prev_highlight = { NO_SQUARE, NO_SQUARE };
                else
                    prev_highlight = { prev_moves.top().from, prev_moves.top().to };

                auto game_status = movgen::make_move(&position, move, &cur_moves);
                prev_moves.push(move);

                board->highlight_prev_move(prev_highlight.x, prev_moves.top().from);
                board->highlight_prev_move(prev_highlight.y, prev_moves.top().to);

                switch (game_status)
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
                return;
            }
        }
    }
    else
        board->select_square(mouse_pos);
}

void Chess::undo_move()
{
    if (!prev_moves.empty())
    {
        sf::Vector2<uint16_t> prev_highlight = { prev_moves.top().from, prev_moves.top().to };

        movgen::undo_move(&position, prev_moves.top());
        cur_moves = position.side_to_move == movgen::WHITE ? movgen::generate_all_moves<movgen::WHITE>(position) : movgen::generate_all_moves<movgen::BLACK>(position);
        cur_moves = movgen::get_legal_moves(position, *cur_moves);

        prev_moves.pop();

        sf::Vector2<uint16_t> new_highlight;
        if (prev_moves.empty())
            new_highlight = { NO_SQUARE, NO_SQUARE };
        else
            new_highlight = { prev_moves.top().from, prev_moves.top().to };

        board->highlight_prev_move(prev_highlight.x, new_highlight.x);
        board->highlight_prev_move(prev_highlight.y, new_highlight.y);
    }
}

void Chess::get_sel_moves()
{
    // Filter selected piece moves
    int selected = board->get_selected_square();
    selected_piece_moves.clear();
    if (selected != -1)
    {
        // Check if there is a piece on that square
        if (position.pieces[movgen::ALL_PIECES] & (1ull << selected))
        {
            for (auto& move : *cur_moves)
            {
                if (move.from == selected)
                {
                    selected_piece_moves.push_back(move);
                }
            }
        }
    }
}
