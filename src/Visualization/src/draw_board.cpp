#include "../headers/draw_board.hpp"
#include <cassert>

Board::Board(sf::Vector2u window_size, bool flipped)
    : flipped(flipped)
{
    this->window_size = window_size;
    label_font.loadFromFile("../data/inconsolata.ttf");

    squares.setUsage(sf::VertexBuffer::Static);
    squares.setPrimitiveType(sf::Quads);
    squares.create(64 * 4);

    this->resize(window_size);

    w_pawn_texture = this->parse_svg_file("../data/w_pn.svg");
    b_pawn_texture = this->parse_svg_file("../data/b_pn.svg");

    w_knight_texture = this->parse_svg_file("../data/w_kn.svg");
    b_knight_texture = this->parse_svg_file("../data/b_kn.svg");

    w_bishop_texture = this->parse_svg_file("../data/w_bs.svg");
    b_bishop_texture = this->parse_svg_file("../data/b_bs.svg");

    w_rook_texture = this->parse_svg_file("../data/w_rk.svg");
    b_rook_texture = this->parse_svg_file("../data/b_rk.svg");

    w_queen_texture = this->parse_svg_file("../data/w_qn.svg");
    b_queen_texture = this->parse_svg_file("../data/b_qn.svg");

    w_king_texture = this->parse_svg_file("../data/w_kg.svg");
    b_king_texture = this->parse_svg_file("../data/b_kg.svg");
}

void Board::resize(sf::Vector2u new_window_size)
{
    float new_size = std::min(new_window_size.x, new_window_size.y);

    this->board_size = static_cast<int>(static_cast<float>(new_size) * board_scale * inner_board_scale);
    this->board_offset = {(new_window_size.x - this->board_size) / 2.0f,
                          (new_window_size.y - this->board_size) / 2.0f};
    this->square_size = board_size / 8;

    place_border();
    place_labels();
    place_squares();
}

void Board::draw_board(sf::RenderWindow *window, movgen::BoardPosition *pos)
{
    window->draw(squares);
    window->draw(char_label);
    window->draw(number_label);
    window->draw(inner);
    window->draw(outer);

    this->draw_pieces(window, pos);
}

void Board::draw_piece_moves(sf::RenderWindow *window, std::vector<movgen::Move> &moves)
{
    sf::CircleShape move_dot;
    move_dot.setFillColor({155, 155, 155, 0});
    move_dot.setOutlineColor({200, 200, 200, 220});
    move_dot.setOutlineThickness(square_size * 0.15f);
    move_dot.setRadius(square_size / 2.0f * 0.5f);

    for (auto &move : moves)
    {
        sf::Vector2f dot_pos = board_to_screen({
            square_size * (move.to % 8),
            square_size * (move.to / 8),
        });
        move_dot.setPosition({dot_pos.x + (square_size / 2.0f - move_dot.getRadius()),
                              dot_pos.y + (square_size / 2.0f - move_dot.getRadius())});

        window->draw(move_dot);
    }
}

void Board::flip_board()
{
    this->flipped = !this->flipped;
    this->place_labels();
    this->place_squares();
}

bool Board::is_flipped()
{
    return flipped;
}

void Board::select_square(sf::Vector2i mouse_pos)
{
    assert(within_bounds(mouse_pos.x, mouse_pos.y));

    sf::Vector2f board_pos = screen_to_board(static_cast<sf::Vector2f>(mouse_pos));

    this->select_square(
        static_cast<int>(board_pos.x / this->square_size),
        static_cast<int>(board_pos.y / this->square_size));
}

void Board::select_square(int x_pos, int y_pos)
{
    assert(x_pos < 8 && x_pos >= 0 && y_pos < 8 && y_pos >= 0);

    int new_select = y_pos * 8 + x_pos;
    if (new_select == this->selected_square)
    {
        this->selected_square = -1;
    }
    else
    {
        this->selected_square = new_select;
    }

    this->place_squares();
}

void Board::deselect_square()
{
    this->selected_square = -1;
    this->place_squares();
}

int Board::get_selected_square()
{
    return this->selected_square;
}

sf::Vector2f Board::screen_to_board(sf::Vector2f screen_pos)
{
    // TODO: finish this
    sf::Vector2f ret_vec;

    float scale = inner_board_scale * board_scale;

    if (flipped)
    {
        ret_vec.x = (screen_pos.x - board_offset.x);
        ret_vec.y = (screen_pos.y - board_offset.y);
    }
    else
    {
        ret_vec.x = board_size - (screen_pos.x - board_offset.x);
        ret_vec.y = board_size - (screen_pos.y - board_offset.y);
    }

    return ret_vec;
}

sf::Vector2f Board::board_to_screen(sf::Vector2f board_pos)
{
    // TODO: finish this
    sf::Vector2f ret_vec;

    if (flipped)
    {
        ret_vec.x = (board_pos.x + board_offset.x);
        ret_vec.y = (board_pos.y + board_offset.y);
    }
    else
    {
        ret_vec.x = board_size - (board_pos.x - board_offset.x) - square_size;
        ret_vec.y = board_size - (board_pos.y - board_offset.y) - square_size;
    }

    return ret_vec;
}

bool Board::within_bounds(float mouse_x, float mouse_y)
{
    return (mouse_x >= board_offset.x && mouse_x <= (board_offset.x + board_size) &&
            mouse_y >= board_offset.y && mouse_y <= (board_offset.y + board_size));
}

float Board::get_square_size()
{
    return this->square_size;
}

float Board::get_size()
{
    return board_size;
}

void Board::place_border()
{
    sf::Vector2f board_center = {window_size.x / 2.0f, window_size.y / 2.0f};
    sf::Vector2f center_origin = {board_size / 2.0f, board_size / 2.0f};

    outer.setSize({board_size, board_size});
    inner.setSize({board_size, board_size});

    outer.setOrigin(center_origin);
    inner.setOrigin(center_origin);

    outer.setScale({1 / inner_board_scale, 1 / inner_board_scale});

    outer.setPosition(board_center);
    inner.setPosition(board_center);

    outer.setOutlineThickness(window_size.x / 250.0f);
    inner.setOutlineThickness(window_size.x / 250.0f);

    outer.setOutlineColor(sf::Color::Black);
    inner.setOutlineColor(sf::Color::Black);

    outer.setFillColor(sf::Color::Transparent);
    inner.setFillColor(sf::Color::Transparent);
}

void Board::place_labels()
{
    // Set label size to half of distance between borders
    float label_size = (board_size * (1 - inner_board_scale)) * 0.5f;
    float char_width = (sf::Text("a", label_font)).getLocalBounds().width;

    char_label.setFont(label_font);
    char_label.setFillColor(sf::Color::Black);
    char_label.setLetterSpacing(13.0f);
    char_label.setCharacterSize(label_size);
    char_label.setPosition({(window_size.x - board_size - char_width + square_size) / 2.0f - char_label.getLetterSpacing() / 2.0f,
                            (window_size.y + board_size) / 2.0f - char_label.getLetterSpacing() / 2.0f});

    number_label.setFont(label_font);
    number_label.setFillColor(sf::Color::Black);
    number_label.setLetterSpacing(13.0f);
    number_label.setCharacterSize(label_size);
    number_label.setPosition({(window_size.x - board_size - label_size * 2.5f) / 2.0f,
                              (window_size.y + board_size + char_width - square_size) / 2.0f + number_label.getLetterSpacing() / 2.0f});
    number_label.setRotation(270);

    if (!this->flipped)
    {
        char_label.setString("abcdefgh");
        number_label.setString("12345678");
    }
    else
    {
        char_label.setString("hgfedcba");
        number_label.setString("87654321");
    }
}

void Board::place_squares()
{
    sf::Color cur_col;
    sf::Vertex *squares = new sf::Vertex[64 * 4];

    for (int i = 0; i < 64; i++)
    {
        squares[i * 4 + 0].position = {
            board_offset.x + (i % 8 + 0) * (square_size),
            board_offset.y + (i / 8 + 0) * (square_size)};
        squares[i * 4 + 1].position = {
            board_offset.x + (i % 8 + 1) * (square_size),
            board_offset.y + (i / 8 + 0) * (square_size)};
        squares[i * 4 + 2].position = {
            board_offset.x + (i % 8 + 1) * (square_size),
            board_offset.y + (i / 8 + 1) * (square_size)};
        squares[i * 4 + 3].position = {
            board_offset.x + (i % 8 + 0) * (square_size),
            board_offset.y + (i / 8 + 1) * (square_size)};

        //Alternate coloring for even rows and columns
        cur_col = (i % 2) ^ (i % 16 >= 8) ? board_green : board_pale;

        if (flipped && i == this->selected_square ||
            !flipped && (63 - i) == this->selected_square)
        {
            cur_col = (i % 2) ^ (i % 16 >= 8) ? selected_dark : selected_light;
        }

        squares[i * 4 + 0].color = cur_col;
        squares[i * 4 + 1].color = cur_col;
        squares[i * 4 + 2].color = cur_col;
        squares[i * 4 + 3].color = cur_col;
    }
    this->squares.update(squares);
}

void Board::draw_pieces(sf::RenderWindow *window, movgen::BoardPosition *pos)
{
    movgen::Piece cur_piece;
    sf::RectangleShape piece;
    piece.setSize({square_size, square_size});

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            movgen::Piece cur_piece = movgen::get_piece(*pos, i * 8 + j);

            if (static_cast<int>(cur_piece) != 0)
            {
                sf::Texture *cur_texture = get_piece_texture(cur_piece);

                piece.setTexture(cur_texture);
                piece.setPosition(this->board_to_screen({(j)*square_size, (i)*square_size}));
                // knight.setFillColor(sf::Color(0, 0, 0, 125));
                window->draw(piece);
            }
        }
    }
}

sf::Texture *Board::get_piece_texture(movgen::Piece piece)
{
    switch (piece)
    {
    case movgen::Piece::B_KING:
        return &b_king_texture;
    case movgen::Piece::W_KING:
        return &w_king_texture;
    case movgen::Piece::B_QUEEN:
        return &b_queen_texture;
    case movgen::Piece::W_QUEEN:
        return &w_queen_texture;
    case movgen::Piece::B_ROOK:
        return &b_rook_texture;
    case movgen::Piece::W_ROOK:
        return &w_rook_texture;
    case movgen::Piece::B_BISHOP:
        return &b_bishop_texture;
    case movgen::Piece::W_BISHOP:
        return &w_bishop_texture;
    case movgen::Piece::B_KNIGHT:
        return &b_knight_texture;
    case movgen::Piece::W_KNIGHT:
        return &w_knight_texture;
    case movgen::Piece::B_PAWN:
        return &b_pawn_texture;
    case movgen::Piece::W_PAWN:
        return &w_pawn_texture;
    default:
        return nullptr;
    }

    return nullptr;
}

sf::Texture Board::parse_svg_file(const char *filename)
{
    NSVGimage *image_path;
    image_path = nsvgParseFromFile(filename, "px", 96.0f);

    if (!std::filesystem::exists(filename))
    {
        std::string err_string = "File not found";
        throw std::runtime_error(err_string.c_str());
    }

    static NSVGrasterizer *rast = nullptr;
    if (rast == nullptr)
    {
        rast = nsvgCreateRasterizer();
    }

    int w = static_cast<int>(this->square_size), h = static_cast<int>(this->square_size);
    float scale = this->square_size / image_path->width;
    // int w = 35; int h = 35;
    unsigned char *image = static_cast<unsigned char *>(malloc(w * h * 4));
    nsvgRasterize(rast, image_path, 0, 0, scale, image, w, h, w * 4);

    sf::Image sfml_image;
    sfml_image.create(w, h, image);

    nsvgDelete(image_path);

    sf::Texture image_texture;
    image_texture.create(w, h);
    image_texture.update(sfml_image);
    image_texture.setSmooth(false);

    return image_texture;
}
