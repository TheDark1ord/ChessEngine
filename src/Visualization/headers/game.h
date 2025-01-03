#include <SFML/Graphics.hpp>

#include <cstring>
#include <iostream>
#include <chrono>
#include <stack>

#include "draw_board.hpp"
#include "event_handler.hpp"

#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include "MagicNumbers.h"

using namespace std::chrono_literals;

class Chess
{
public:
    Chess(sf::Vector2u window_size);
    Chess(sf::Vector2u window_size, std::string fen);

    void loop();

private:
    const char *fen_string;
    const sf::Color bg_color = sf::Color(255, 255, 240);

    sf::RenderWindow window;
    sf::Image game_icon;
    std::string data_dir;
    Board* board;

    movgen::BoardPosition position;

    std::stack<movgen::Move> prev_moves;

    std::vector<movgen::Move>* cur_moves;
    std::vector<movgen::Move> selected_piece_moves;

    void handle_event(sf::Event ev);
    void display();
};
