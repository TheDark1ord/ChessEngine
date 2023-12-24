#include "../headers/draw_board.hpp"

Board::Board(sf::Vector2u window_size, bool flipped)
    :flipped(flipped)
{
    this->window_size = window_size;
    label_font.loadFromFile("../data/arial.ttf");

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
    this->board_size = static_cast<int>(static_cast<float>(new_window_size.x)
        * board_scale * inner_board_scale);
    this->board_offset = (new_window_size.x - this->board_size) / 2.0f;
    this->cell_size = board_size / 8;

    place_border();
    place_labels();
    place_squares();
}

void Board::draw_board(sf::RenderWindow* window, movgen::BoardPosition* pos)
{
    window->draw(squares);
    window->draw(char_label);
    window->draw(number_label);
    window->draw(inner);
    window->draw(outer);

    this->draw_pieces(window, pos);
}

sf::Vector2f Board::screen_to_board(sf::Vector2f screen_pos)
{
    //TODO: finish this
    throw std::logic_error("Not implemented");
    return sf::Vector2f();
}

sf::Vector2f Board::board_to_screen(sf::Vector2f board_pos)
{
    //TODO: finish this
    sf::Vector2f ret_vec;

    ret_vec.x = board_pos.x + board_offset;
    ret_vec.y = board_pos.y + board_offset;

    return ret_vec;
}

void Board::place_border()
{
    sf::Vector2f board_center = { window_size.x / 2.0f, window_size.y / 2.0f };
    sf::Vector2f center_origin = { board_size / 2.0f, board_size / 2.0f };

    outer.setSize({ board_size, board_size });
    inner.setSize({ board_size, board_size });

    outer.setOrigin(center_origin);
    inner.setOrigin(center_origin);

    outer.setScale({ 1 / inner_board_scale, 1 / inner_board_scale });

    outer.setPosition(board_center);
    inner.setPosition(board_center);

    outer.setOutlineThickness(4);
    inner.setOutlineThickness(4);

    outer.setOutlineColor(sf::Color::Black);
    inner.setOutlineColor(sf::Color::Black);

    outer.setFillColor(sf::Color::Transparent);
    inner.setFillColor(sf::Color::Transparent);
}

void Board::place_labels()
{
    static float label_size = (board_size * (1 - inner_board_scale)) * 0.5f;
    //static float label_size = 46;

    static float char_width = (sf::Text("a", label_font)).getLocalBounds().width;

    char_label.setFont(label_font);
    char_label.setColor(sf::Color::Black);
    char_label.setLetterSpacing(22.5f);
    char_label.setCharacterSize(label_size);
    char_label.setPosition({
        (window_size.x - board_size - char_width + cell_size) / 2.0f,
        (window_size.y + board_size) / 2.0f
        });

    number_label.setFont(label_font);
    number_label.setColor(sf::Color::Black);
    number_label.setLetterSpacing(22.2f);
    number_label.setCharacterSize(label_size);
    number_label.setPosition({
        (window_size.x - board_size - label_size * 2.5f) / 2.0f,
        (window_size.y + board_size + char_width - cell_size) / 2.0f
        });
    number_label.setRotation(270);

    if (!flipped)
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
    sf::Vertex* squares = new sf::Vertex[64 * 4];

    for (int i = 0; i < 64; i++) {
        squares[i * 4 + 0].position = {
            board_offset + (i % 8 + 0) * (cell_size),
            board_offset + (i / 8 + 0) * (cell_size)
        };
        squares[i * 4 + 1].position = {
            board_offset + (i % 8 + 1) * (cell_size),
            board_offset + (i / 8 + 0) * (cell_size)
        };
        squares[i * 4 + 2].position = {
            board_offset + (i % 8 + 1) * (cell_size),
            board_offset + (i / 8 + 1) * (cell_size)
        };
        squares[i * 4 + 3].position = {
            board_offset + (i % 8 + 0) * (cell_size),
            board_offset + (i / 8 + 1) * (cell_size)
        };

        cur_col = (i % 2) ^ (i % 16 >= 8) ?
            board_green :
            board_pale;

        squares[i * 4 + 0].color = cur_col;
        squares[i * 4 + 1].color = cur_col;
        squares[i * 4 + 2].color = cur_col;
        squares[i * 4 + 3].color = cur_col;
    }
    this->squares.update(squares);
}

void Board::draw_pieces(sf::RenderWindow* window, movgen::BoardPosition* pos)
{
    movgen::Piece cur_piece;
    sf::RectangleShape piece;
    piece.setSize({ cell_size, cell_size });

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            movgen::Piece cur_piece = movgen::get_piece(*pos, i * 8 + j);

            if (static_cast<int>(cur_piece) != 0)
            {
                sf::Texture* cur_texture = get_piece_texture(cur_piece);

                piece.setTexture(cur_texture);

                if (!flipped)
                {
                    piece.setPosition(this->board_to_screen({ (7 - j) * cell_size, (7 - i) * cell_size }));
                }
                else
                {
                    piece.setPosition(this->board_to_screen({ j * cell_size, i * cell_size }));
                }
                //knight.setFillColor(sf::Color(0, 0, 0, 125));
                window->draw(piece);
            }
        }
    }
}

sf::Texture* Board::get_piece_texture(movgen::Piece piece)
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
    }

    return nullptr;
}

sf::Texture Board::parse_svg_file(const char* filename)
{
    NSVGimage* image_path;
    image_path = nsvgParseFromFile(filename, "px", 96.0f);

    if (!std::filesystem::exists(filename)) {
        std::string err_string = "File not found";
        throw std::runtime_error(err_string.c_str());
    }

    static NSVGrasterizer* rast = nullptr;
    if (rast == nullptr)
    {
        rast = nsvgCreateRasterizer();
    }

    int w = static_cast<int>(this->cell_size), h = static_cast<int>(this->cell_size);
    float scale = this->cell_size / image_path->width;
    //int w = 35; int h = 35;
    unsigned char* image = static_cast<unsigned char*>(malloc(w * h * 4));
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
