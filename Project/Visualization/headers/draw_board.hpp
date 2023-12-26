#ifndef DRAW_BOARD_H
#define DRAW_BOARD_H

#include <SFML/Graphics.hpp>

#include <string.h>
#include <math.h>
#include <filesystem>
#include <iostream>
#include <regex>

#include "nanosvg.h"
#include "nanosvgrast.h"

#include "MovgenTypes.h"

#define board_green sf::Color(118, 150, 86)
#define board_pale sf::Color(238, 238, 210)
#define board_yellow sf::Color(186, 202, 68)

#define selected_light sf::Color(225, 234, 162)
#define selected_dark sf::Color(165, 190, 101)

/// Let's call it margin of the graphics relative to the board size
// This scale of the first outline
const float board_scale = 0.9f;
// This scale of the first outline
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
    Board(sf::Vector2u window_size, bool flipped = false);

    void resize(sf::Vector2u new_window_size);
    void draw_board(sf::RenderWindow* window, movgen::BoardPosition* pos);

    void flip_board();

    void select_square(int x_pos, int y_pos);
    int get_selected_square();

    // Practically usless now< but may become useeful later
    sf::Vector2f screen_to_board(sf::Vector2f screen_pos);
    sf::Vector2f board_to_screen(sf::Vector2f board_pos);

    float get_cell_size();
private:

    // Theese functions adjust settings for the
    // entities below, but do not draw any of them.
    // This way I do not have to change theese settings for
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
    void draw_pieces(sf::RenderWindow* window, movgen::BoardPosition* pos);
    sf::Texture* get_piece_texture(movgen::Piece piece);

    /// @brief Rasterize the given svg file
    /// @param filename path to file
    /// @return Texture the size of a single board cell, with the image from the provided file
    sf::Texture parse_svg_file(const char* filename);

    sf::Vector2u window_size;
    float board_size;

    // Offset relative to (0, 0)
    float board_offset;
    float cell_size;

    bool flipped;
    int selected_square = -1;

    // If the window has not 1:1 ratio, then this variable hold the offset
    // to center all the drawings
    sf::Vector2f window_offset;

    sf::Texture w_pawn_texture;
    sf::Texture b_pawn_texture;

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