#include <iostream>
#include "../include/MoveGeneration.h"

inline void movgen::set_bit(bitboard* b, bpos pos)
{
    *b |= 1 << pos;
}

inline void movgen::cleat_bit(bitboard* b, bpos pos)
{
    *b |= ~(1 << pos);
}

inline void movgen::flip_bit(bitboard* b, bpos pos)
{
    *b ^= 1 << pos;
}

inline bool movgen::read_bit(bitboard& b, bpos pos)
{
    return b & (1 << pos);
}

size_t std::hash<movgen::BoardPosition>::operator()(movgen::BoardPosition const& p) const noexcept
{
    //TODO:
    size_t hash = 0;
    return size_t();
}

movgen::BoardHash::BoardHash(BoardPosition& pos)
{
    this->hash = std::hash<movgen::BoardPosition>{}(pos);
}

size_t movgen::BoardHash::operator()(BoardHash const& h) const
{
    return this->hash;
}

movgen::BoardPosition movgen::board_from_fen(std::string fen)
{
    static std::regex fen_regex(movgen::fen_regex_string, std::regex_constants::ECMAScript);
    if (!std::regex_match(fen, fen_regex))
    {
        throw std::runtime_error("Invalid fen string");
    }

    movgen::BoardPosition return_pos;
    int it = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isdigit(fen[it])) {
                j += static_cast<int>(fen[it]);
                it++;

                if (j >= 8)
                    throw std::runtime_error("Invalid fen");

                continue;
            }

            switch (fen[it])
            {
            case 'P':
                set_bit(&return_pos.w_pawns, 63 - (i * 8 + j));
                break;
            case 'p':
                set_bit(&return_pos.b_pawns, 63 - (i * 8 + j));
                break;
            case 'N':
                set_bit(&return_pos.w_knights, 63 - (i * 8 + j));
                break;
            case 'n':
                set_bit(&return_pos.b_knights, 63 - (i * 8 + j));
                break;
            case 'B':
                set_bit(&return_pos.w_bishops, 63 - (i * 8 + j));
                break;
            case 'b':
                set_bit(&return_pos.b_bishops, 63 - (i * 8 + j));
                break;
            case 'R':
                set_bit(&return_pos.w_rooks, 63 - (i * 8 + j));
                break;
            case 'r':
                set_bit(&return_pos.b_rooks, 63 - (i * 8 + j));
                break;
            case 'Q':
                set_bit(&return_pos.w_queens, 63 - (i * 8 + j));
                break;
            case 'q':
                set_bit(&return_pos.b_queens, 63 - (i * 8 + j));
                break;
            case 'K':
                set_bit(&return_pos.w_kings, 63 - (i * 8 + j));
                break;
            case 'k':
                set_bit(&return_pos.b_kings, 63 - (i * 8 + j));
                break;


            case '/':
                throw std::runtime_error("Invalid fen");
            }
            it++;
        }
    }

    // Bitboard, representing all 1st and 8th rows
    // set to 1
    bitboard top_and_bottom = 18374686479671623935;
    // Test that no pawns are on the first and last rows
    if (return_pos.w_pawns | top_and_bottom ||
        return_pos.b_pawns | top_and_bottom)
    {
        throw std::runtime_error("Invalid fen");
    }

    // Assign composite bitboards
    return_pos.b_pieces = return_pos.b_kings |
        return_pos.b_queens | return_pos.b_rooks |
        return_pos.b_bishops | return_pos.b_knights |
        return_pos.b_pawns;
    return_pos.w_pieces = return_pos.w_kings |
        return_pos.w_queens | return_pos.w_rooks |
        return_pos.w_bishops | return_pos.w_knights |
        return_pos.w_pawns;
    return_pos.all_pieces = return_pos.b_pieces | return_pos.w_pieces;

    // Scip spaces
    while (fen[++it] == ' ') {};
    return_pos.move = fen[it] == 'w';

    // Scip spaces
    while (fen[++it] == ' ') {}
    while (fen[it] != ' ')
    {
        switch (fen[it++])
        {
        case 'K':
            return_pos.castling |= 1;
            break;
        case 'Q':
            return_pos.castling |= 2;
            break;
        case 'k':
            return_pos.castling |= 4;
            break;
        case 'q':
            return_pos.castling |= 8;
            break;

        default:
            // No castling
            goto EnPassant;
        }
    }

EnPassant:
    while (fen[++it] == ' ') {}
    if (fen[it] == '-')
    {
        // It is a valid position, but considering, that with correct position
        // a pawn cannot take that en passant
        return_pos.en_passant = 0;
    }
    else {
        return_pos.en_passant = (fen[it] - 'a') + (fen[++it] - '1') * 8;
    }
    // Test that en passant square is on 3rd or 6th row
    if ((1 << return_pos.en_passant) | 18446463698227757055))
    {
        throw std::runtime_error("Invalid fen");
    }

    while (fen[++it] == ' ') {}
    return_pos.halfmove = std::atoi(fen[it]);
    while (fen[++it] == ' ') {}
    return_pos.fullmove = std::atoi(fen[it]);

    return return_pos;
}

void generateKingMoves(movgen::BoardPosition& pos, movgen::movgen::GeneratedMoves* moves)
{

}

void generateQueenMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void generateRookMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void generateBishopMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void generateKnightMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void generatePawnMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

std::string movgen::board_to_fen(movgen::BoardPosition& pos)
{
    return std::string();
}