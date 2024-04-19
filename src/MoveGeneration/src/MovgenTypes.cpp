#include "../include/MovgenTypes.h"
#include "../include/MurmurHash3.h"
#include <cassert>
#include <regex>
#include <stdexcept>

movgen::BoardPosition movgen::board_from_fen(std::string fen)
{
    static std::regex fen_regex(movgen::fen_regex_string);
    if (!std::regex_match(fen, fen_regex))
    {
        throw std::runtime_error("Invalid fen string");
    }

    movgen::BoardPosition return_pos;

    for (unsigned int i = 0; i < movgen::PIECE_NB; i++)
        return_pos.pieces[i] = 0;
    for (unsigned int i = 0; i < 64; i++)
        return_pos.squares[i] = movgen::NO_PIECE;

    int it = 0;
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (isdigit(fen[it]))
            {
                j += static_cast<int>(fen[it] - '0');
                it++;

                if (j > 8)
                    throw std::runtime_error("Invalid fen");

                continue;
            }

            switch (fen[it])
            {
            case 'P':
                bitb::set_bit(&return_pos.pieces[W_PAWN], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_PAWN;
                break;
            case 'p':
                bitb::set_bit(&return_pos.pieces[B_PAWN], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_PAWN;
                break;
            case 'N':
                bitb::set_bit(&return_pos.pieces[W_KNIGHT], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_KNIGHT;
                break;
            case 'n':
                bitb::set_bit(&return_pos.pieces[B_KNIGHT], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_KNIGHT;
                break;
            case 'B':
                bitb::set_bit(&return_pos.pieces[W_BISHOP], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_BISHOP;
                break;
            case 'b':
                bitb::set_bit(&return_pos.pieces[B_BISHOP], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_BISHOP;
                break;
            case 'R':
                bitb::set_bit(&return_pos.pieces[W_ROOK], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_ROOK;
                break;
            case 'r':
                bitb::set_bit(&return_pos.pieces[B_ROOK], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_ROOK;
                break;
            case 'Q':
                bitb::set_bit(&return_pos.pieces[W_QUEEN], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_QUEEN;
                break;
            case 'q':
                bitb::set_bit(&return_pos.pieces[B_QUEEN], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_QUEEN;
                break;
            case 'K':
                bitb::set_bit(&return_pos.pieces[W_KING], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = W_KING;
                break;
            case 'k':
                bitb::set_bit(&return_pos.pieces[B_KING], 63 - (i * 8 + j));
                return_pos.squares[63 - (i * 8 + j)] = B_KING;
                break;

                // This is forward slash(/) this shid does not work with char def
            case '/':
                if (j != 0)
                {
                    throw std::runtime_error(
                        "Invalid fen(while parsing piece information)");
                }
                j--;
                break;
            }
            it++;
        }
    }

    // Bitboard, representing 1st and last rows
    constexpr bitboard top_and_bottom = bitb::Rank[0] | bitb::Rank[7];
    // Test that no pawns are on the first and last rows
    if (return_pos.pieces[W_PAWN] & top_and_bottom ||
        return_pos.pieces[B_PAWN] & top_and_bottom)
    {
        throw std::runtime_error("Impossible position");
    }

    // Assign composite bitboards
    return_pos.pieces[BLACK_PIECES] = return_pos.pieces[B_KING] | return_pos.pieces[B_QUEEN] |
                                      return_pos.pieces[B_ROOK] | return_pos.pieces[B_BISHOP] |
                                      return_pos.pieces[B_KNIGHT] | return_pos.pieces[B_PAWN];
    return_pos.pieces[WHITE_PIECES] = return_pos.pieces[W_KING] | return_pos.pieces[W_QUEEN] |
                                      return_pos.pieces[W_ROOK] | return_pos.pieces[W_BISHOP] |
                                      return_pos.pieces[W_KNIGHT] | return_pos.pieces[W_PAWN];
    return_pos.pieces[ALL_PIECES] = return_pos.pieces[BLACK_PIECES] | return_pos.pieces[WHITE_PIECES];

    // Scip spaces
    while (fen[++it] == ' ')
    {
    };
    return_pos.side_to_move = (movgen::Color)(fen[it] == 'w');

    // Skip spaces
    while (fen[++it] == ' ')
    {
    }

    unsigned int castling = NO_CASTLING;
    while (fen[it] != ' ')
    {
        switch (fen[it++])
        {
        case 'K':
            castling |= WHITE_SHORT;
            break;
        case 'Q':
            castling |= WHITE_LONG;
            break;
        case 'k':
            castling |= BLACK_SHORT;
            break;
        case 'q':
            castling |= BLACK_LONG;
            break;

        default:
            return_pos.castling_rights = NO_CASTLING;
        }
    }
    return_pos.castling_rights = static_cast<CastlingRights>(castling);

EnPassant:
    while (fen[++it] == ' ')
    {
    }

    if (fen[it] == '-')
    {
        // It is a valid position, but considering, that with correct position
        // a pawn cannot take that en passant
        return_pos.en_passant = 0;
    }
    else
    {
        return_pos.en_passant = (fen[it] - 'a') + (fen[it + 1] - '1') * 8;
        it++;
    }
    // Test that en passant square is on 3rd or 6th row
    // TODO: possible bug here(maybe should sub 1 from return_pos.en_passant)
    if (bitb::sq_bitb(return_pos.en_passant) & (bitb::Rank[2] | bitb::Rank[5]))
    {
        throw std::runtime_error("Invalid position");
    }

    while (fen[++it] == ' ')
    {
    }
    return_pos.halfmove = fen[it] - '0';
    while (fen[++it] == ' ')
    {
    }
    return_pos.fullmove = fen[it] - '0';

    return return_pos;
}

std::string movgen::board_to_fen(movgen::BoardPosition &pos)
{
    // TODO: board_to_fen
    throw std::runtime_error("Not implemented");
    return std::string();
}

movgen::MoveType movgen::Move::get_type() const
{
    if (this->move_data == 0)
        return movgen::REGULAR;
    else if (this->move_data & 0x0F)
        return this->move_data & 0xF0 ? movgen::PROMOTION_CAPTURE : movgen::CAPTURE;
    else if (this->move_data & 0xF0)
        return movgen::PROMOTION;
    else if (this->move_data & 0x100)
        return movgen::DOUBLE_MOVE;
    else if (this->move_data & 0x200)
        return movgen::EN_PASSANT;
    else
        return movgen::CASTLING;
}

movgen::Piece movgen::get_piece(BoardPosition &b_pos, bpos pos)
{
    return b_pos.squares[pos];
}

movgen::PieceType movgen::get_piece_type(BoardPosition &b_pos, bpos pos)
{
    return movgen::get_piece_type(b_pos.squares[pos]);
}

movgen::Color movgen::get_piece_color(movgen::Piece piece)
{
    assert(piece != 0);
    assert(piece <= 14);
    return static_cast<movgen::Color>(piece >> 3);
}

movgen::Move::Move(Piece piece, bpos from, bpos to)
    : piece(piece), from(from), to(to), move_data(0)
{
}

movgen::Move::Move(Piece piece, bpos from, bpos to, unsigned char capture, unsigned char promotion, bool double_move, bool en_passant, unsigned char castling)
    : piece(piece), from(from), to(to), move_data(0)
{
    this->move_data |= piece & 0x0F;
    this->move_data |= promotion & 0x0F << 4;
    this->move_data |= double_move << 8;
    this->move_data |= en_passant << 9;
    this->move_data |= castling << 10;
}

movgen::BoardHash::BoardHash(BoardPosition &pos)
{
    this->hash = std::hash<movgen::BoardPosition>{}(pos);
}

bool movgen::BoardHash::operator==(const movgen::BoardHash& other) const
{
    return this->hash == other.hash;
}

size_t std::hash<movgen::BoardHash>::operator()(
    movgen::BoardHash const &p) const noexcept
{
    return p.hash;
}

size_t std::hash<movgen::BoardPosition>::operator()(
    movgen::BoardPosition const &p) const noexcept
{
    // Concatenate all the variables into a single array
    uint64_t *data = new uint64_t[movgen::PIECE_NB + 1];

    std::memcpy(data, p.pieces, movgen::PIECE_NB * sizeof(uint64_t));
    data[movgen::PIECE_NB] = (static_cast<uint8_t>(p.side_to_move)) | (static_cast<uint8_t>(p.castling_rights) << 8) |
                             (static_cast<uint16_t>(p.en_passant) << 16);

    size_t hash[2];
    MurmurHash3_x64_128(data, (movgen::PIECE_NB + 1) * sizeof(uint64_t), 0x00000000, &hash);

    return hash[0];
}