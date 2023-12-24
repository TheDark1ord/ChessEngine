#include <SFML/Graphics.hpp>
#include <Windows.h>

//#define NANOSVG_IMPLEMENTATION
//#define NANOSVGRAST_IMPLEMENTATION

#include "headers/event_handler.hpp"
#include "headers/draw_board.hpp"

#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include "MagicNumbers.h"

#ifdef NDEBUG
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
#else
int main()
#endif
{
#ifndef NDEBUG
    /* code */
    HWND hWnd = GetConsoleWindow();
    ShowWindow(hWnd, SW_HIDE);
#endif //DEBUG


    sf::RenderWindow window(sf::VideoMode(1280, 1280), "Chess");
    window.setFramerateLimit(60.0f);

    sf::Image icon;
    icon.loadFromFile("../data/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    movgen::BoardPosition position;
    try {
        position = movgen::board_from_fen(
            "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
        );
    }
    catch (std::exception& e) {
        std::cout << "Runtime exception while parsing fen string:\n";
        std::cout << e.what();
    }

    Board* board;
    try {
        board = new Board(window.getSize());
    }
    catch (std::exception& e) {
        MessageBox(NULL, e.what(), "Error", MB_OK);
    }

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            handle_event(event, &window);
        }

        window.clear(sf::Color(255, 255, 240));

        board->draw_board(&window, &position);

        window.display();
    }

    return 0;
}