#ifndef DRAW_BOARD_H
#define DRAW_BOARD_H

#include <SFML/Graphics.hpp>

#include <string.h>
#include <math.h>
#include <filesystem>
#include <iostream>

#include "nanosvg.h"
#include "nanosvgrast.h"

#define board_green sf::Color(118, 150, 86)
#define board_pale sf::Color(238, 238, 210)
#define board_yellow sf::Color(186, 202, 68)

// Let's call it margin of the graphics relative to the board size
const float board_scale = 0.9f;
const float inner_board_scale = 0.9f;

inline float map_range(float input, float in_start, float in_end,
    float out_start, float out_end)
{
    return out_start + ((out_end - out_start) / (in_end - in_start))
        * (input - in_start);
}

class Board
{
public:
    Board(sf::Vector2u window_size);

    void resize(sf::Vector2u new_window_size);
    void draw_board(sf::RenderWindow* window);

    sf::Vector2f screen_to_board(sf::Vector2f screen_pos);
    sf::Vector2f board_to_screen(sf::Vector2f board_pos);

private:

    // Theese functions adjust settings for the
    // entities below, but do not draw any of them
    // this way I do not have to change theese settings for
    // each frame
    void place_border();
    void place_labels();
    void place_squares();

    sf::Font label_font;
    sf::Text char_label;
    sf::Text number_label;

    sf::RectangleShape inner;
    sf::RectangleShape outer;

    sf::VertexBuffer squares;

    //TODO: add bitboard struct reference
    void draw_pieces(sf::RenderWindow* window);

    sf::Texture parse_svg_file(const char* filename);

    sf::Vector2u window_size;
    float board_size;
    float board_offset;
    float cell_size;

    // If the window has not 1:1 ratio, then this variable hold the offset
    // to center all the drawings
    sf::Vector2f window_offset;

    sf::Texture w_knight_texture;
    sf::Texture b_knight_texture;

    sf::Texture w_bishop_texture;
    sf::Texture b_bishop_texture;

    sf::Texture w_rook_texture;
    sf::Texture b_rook_texture;

    sf::Texture w_queen_texture;
    sf::Texture b_queen_texture;

    sf::Texture w_king_texture;
    sf::Texture b_king_texture;
};

#endif