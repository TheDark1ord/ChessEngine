#include <SFML/Graphics.hpp>
#include <Windows.h>

#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION

#include "headers/event_handler.hpp"
#include "headers/draw_board.hpp"

//int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 800), "SFML works!");
    window.setFramerateLimit(60.0f);

    Board* board;
    try {
        board = new Board(window.getSize());
    } catch (std::exception &e) {
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

        board->draw_board(&window);

        window.display();
    }

    return 0;
}