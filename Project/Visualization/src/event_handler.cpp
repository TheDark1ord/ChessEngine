#include "../headers/event_handler.hpp"
#include "../headers/draw_board.hpp"

void handle_event(sf::Event ev, sf::RenderWindow *window)
{
    switch (ev.type)
    {
    case sf::Event::Closed:
        window->close();
        break;
    case sf::Event::Resized:
        // TODO: Fix this

        break;
    }
}
