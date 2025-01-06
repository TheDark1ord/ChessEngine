#include "../headers/event_handler.hpp"
#include "../headers/draw_board.hpp"

void handle_event(sf::Event ev, sf::RenderWindow *window)
{
    switch (ev.type)
    {
    case sf::Event::Closed:
        window->close();
        break;

    case sf::Event::KeyReleased:
        switch (ev.key.code)
        {
        case sf::Keyboard::R:
            board->flip_board();
            break;

        default:
            break;
        }
        break;

    case sf::Event::MouseButtonPressed:
        switch (ev.mouseButton.button)
        {
        case sf::Mouse::Left:
        {
            sf::Vector2f mouse_pos = board->screen_to_board(static_cast<sf::Vector2f>(sf::Mouse::getPosition(*window)));
            board->select_square(
                static_cast<int>(mouse_pos.x / board->get_square_size()),
                static_cast<int>(mouse_pos.y / board->get_square_size()));
            break;
        }

        default:
            break;
        }
    case sf::Event::Resized:
        // TODO: Fix this

        break;
    default:
        break;
    }
}
