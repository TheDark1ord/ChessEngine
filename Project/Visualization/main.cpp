#include <SFML/Graphics.hpp>
#include <Windows.h>

#include <cstring>
//#define NANOSVG_IMPLEMENTATION
//#define NANOSVGRAST_IMPLEMENTATION

#include "headers/event_handler.hpp"
#include "headers/draw_board.hpp"

#include "MoveGeneration.h"
#include "MovgenTypes.h"
#include "MagicNumbers.h"

/// @brief Catch and process all thrown exceptions, that bubble past main
void terminate_func();

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
#ifndef NDEBUG
    // Show console in debug mode
    AllocConsole();
    freopen("conin$", "r", stdin);
    freopen("conout$", "w", stdout);
    freopen("conout$", "w", stderr);
#endif //DEBUG


    sf::RenderWindow window(sf::VideoMode(1280, 1280), "Chess");
    window.setFramerateLimit(60.0f);

    sf::Image icon;
    icon.loadFromFile("../data/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    movgen::BoardPosition position;

    position = movgen::board_from_fen(
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    );

    board = new Board(window.getSize());

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

void terminate_func()
{
    std::cerr << "terminate called after throwing an instance of ";
    try
    {
        std::rethrow_exception(std::current_exception());
    }
    catch (const std::exception& ex)
    {
        std::cerr << typeid(ex).name() << std::endl;
        std::cerr << "  what(): " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << typeid(std::current_exception()).name() << std::endl;
        std::cerr << " ...something, not an exception, dunno what." << std::endl;
    }
    MessageBox(NULL, "Failed due to an exception", "Error", MB_OK);
    std::cerr << "errno: " << errno << ": " << std::strerror(errno) << std::endl;
    std::abort();
}