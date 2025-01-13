#include <SFML/Graphics.hpp>

#include <stack>
#include <boost/process.hpp>
#include <boost/process/windows.hpp>

#include "draw_board.hpp"

#include "MovgenTypes.h"

#include <Windows.h>

using namespace std::chrono_literals;
namespace bp = boost::process;


enum class GameMode {
    PlayerVPlayer,
    PlayerVEngine
};

class EngineChildProcess
{
public:
	EngineChildProcess();
	~EngineChildProcess();

	std::atomic<bool> engine_ready;
	std::string engine_search(std::string fen);

private:
	//Child process IO
	bp::opstream engine_in;
	bp::ipstream engine_out;

	bp::child engine_process;

	const char* engine_exe_path = ".\\engine.exe";
};

class Chess
{
public:
    Chess(sf::Vector2u window_size, GameMode mode = GameMode::PlayerVPlayer);
    Chess(sf::Vector2u window_size, std::string fen, GameMode mode);

    void loop();

private:
    GameMode mode;
	EngineChildProcess* engine;
	bool players_turn = false;

	void handle_engine_move();

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
    void move_piece(movgen::Move move);
    void update_piece_moves_highlight();

    void handle_resized_event(sf::Event::SizeEvent size);

    void display();
};

