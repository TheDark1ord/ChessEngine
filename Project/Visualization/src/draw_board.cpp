#include "../headers/draw_board.hpp"

Board::Board(sf::Vector2u window_size)
{
    this->window_size = window_size;
    label_font.loadFromFile("../data/arial.ttf");

    squares.setUsage(sf::VertexBuffer::Static);
    squares.setPrimitiveType(sf::Quads);
    squares.create(64 * 4);

    this->resize(window_size);

    w_knight_texture = this->parse_svg_file("../data/23.svg");
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

void Board::draw_board(sf::RenderWindow* window)
{
    window->draw(squares);
    window->draw(char_label);
    window->draw(number_label);
    window->draw(inner);
    window->draw(outer);

    sf::RectangleShape knight;
    knight.setSize({cell_size, cell_size});
    knight.setTexture(&w_knight_texture);
    knight.setPosition(500, 500);
    window->draw(knight);
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
    throw std::logic_error("Not implemented");
    return sf::Vector2f();
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

    char_label.setString("abcdefgh");
    number_label.setString("12345678");
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

sf::Texture Board::parse_svg_file(const char* filename)
{
    NSVGimage* image_path;
    image_path = nsvgParseFromFile(filename, "px", 96);

    if (!std::filesystem::exists(filename)) {
        std::string err_string = "File not found";
        throw std::runtime_error(err_string.c_str());
    }

    struct NSVGrasterizer* rast = nullptr;
    if (rast == nullptr)
    {
        rast = nsvgCreateRasterizer();
    }

    int w = image_path->width, h = image_path->height;
    unsigned char* image = static_cast<unsigned char*>(malloc(w * h * 4));
    nsvgRasterize(rast, image_path, 0, 0, 1, image, w, h, w * 4);

    sf::Texture image_texture;
    image_texture.loadFromMemory(image, w * h * 4);

    nsvgDelete(image_path);

    return image_texture;
}
