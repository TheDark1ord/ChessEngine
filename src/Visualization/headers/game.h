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

enum class GameMode {
    PlayerVPlayer,
    PlayerVEngine
};

class Chess
{
public:
    Chess(sf::Vector2u window_size, GameMode mode = GameMode::PlayerVPlayer);
    Chess(sf::Vector2u window_size, std::string fen, GameMode mode);

    void loop();

private:
    GameMode mode;

    const char *fen_string;
    const sf::Color bg_color = sf::Color(255, 255, 240);

    sf::RenderWindow window;
    sf::Image game_icon;
    Board *board;

    movgen::BoardPosition position;

    std::stack<movgen::Move> prev_moves;

    std::vector<movgen::Move>* cur_moves;
    std::vector<movgen::Move> selected_piece_moves;

    void handle_event(sf::Event ev);

    //Handle event subfunctions
    void reset_move();

    void handle_left_button_press();
    void move_piece();
    void update_piece_moves_highlight();

    void handle_resized_event(sf::Event::SizeEvent size);

    
    void display();
};