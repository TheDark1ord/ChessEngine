#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <SFML/Graphics.hpp>
#include "draw_board.hpp"

// All variables, accessed by this handler are located here
inline Board* board;

void handle_event(sf::Event ev, sf::RenderWindow *window);

#endif