#include <iostream>
#include "../include/MoveGeneration.h"

inline std::vector<bpos> movgen::bitscan(bitboard board)
{
    // https://www.chessprogramming.org/BitScan#Bitscan_by_Modulo
    static constexpr bpos lookup67[68] = {
        64,  0,  1, 39,  2, 15, 40, 23,
        3, 12, 16, 59, 41, 19, 24, 54,
        4, -1, 13, 10, 17, 62, 60, 28,
        42, 30, 20, 51, 25, 44, 55, 47,
        5, 32, -1, 38, 14, 22, 11, 58,
        18, 53, 63,  9, 61, 27, 29, 50,
        43, 46, 31, 37, 21, 57, 52,  8,
        26, 49, 45, 36, 56,  7, 48, 35,
        6, 34, 33, -1
    };

    std::vector<bpos> set_bits;
    // TODO: Test optimal reserve number and probably update to better algorithm
    set_bits.reserve(8);

    while (board != 0)
    {
        set_bits.push_back(lookup67[(board & (~board + 1)) % 67]);
        board &= board - 1;
    }

    return set_bits;
}

Piece movgen::get_piece(BoardPosition& b_pos, bpos pos, unsigned char color)
{
    bitboard mask = 1ull << pos;
    Piece return_piece = static_cast<Piece>(0);

    if (color != 2)
    {
        return_piece = b_pos.w_kings & mask ? Piece::W_KING : return_piece;
        return_piece = b_pos.w_queens & mask ? Piece::W_QUEEN : return_piece;
        return_piece = b_pos.w_rooks & mask ? Piece::W_ROOK : return_piece;
        return_piece = b_pos.w_bishops & mask ? Piece::W_BISHOP : return_piece;
        return_piece = b_pos.w_knights & mask ? Piece::W_KNIGHT : return_piece;
        return_piece = b_pos.w_pawns & mask ? Piece::W_PAWN : return_piece;
    }

    if (color != 1)
    {
        return_piece = b_pos.b_kings & mask ? Piece::B_KING : return_piece;
        return_piece = b_pos.b_queens & mask ? Piece::B_QUEEN : return_piece;
        return_piece = b_pos.b_rooks & mask ? Piece::B_ROOK : return_piece;
        return_piece = b_pos.b_bishops & mask ? Piece::B_BISHOP : return_piece;
        return_piece = b_pos.b_knights & mask ? Piece::B_KNIGHT : return_piece;
        return_piece = b_pos.b_pawns & mask ? Piece::B_PAWN : return_piece;
    }

    return return_piece;
}

size_t std::hash<movgen::BoardPosition>::operator()(movgen::BoardPosition const& p) const noexcept
{
    //TODO: board hash
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

    // Bitboard, representing 1st and last rows
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
    if ((1ull << return_pos.en_passant) | 18446463698227757055)
    {
        throw std::runtime_error("Invalid fen");
    }

    while (fen[++it] == ' ') {}
    return_pos.halfmove = fen[it] - '0';
    while (fen[++it] == ' ') {}
    return_pos.fullmove = fen[it] - '0';

    return return_pos;
}

std::string movgen::board_to_fen(movgen::BoardPosition& pos)
{
    // TODO: board_to_fen
    return std::string();
}

void movgen::generateKingMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void movgen::generateQueenMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{

}

void movgen::generateRookMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{

}

void movgen::generateBishopMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{

}

void movgen::generateKnightMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves)
{

}

void movgen::generatePawnMoves(movgen::BoardPosition& board, movgen::GeneratedMoves* moves)
{
    // Edge pawn captures should be handled separetly
    static constexpr bitboard left_edge = 0x8080808080808080;
    static constexpr bitboard right_edge = 0x0101010101010101;
    // To Handle promotions
    static constexpr bitboard top_edge = 0xff00000000000000;
    static constexpr bitboard bottom_edge = 0x00000000000000ff;

    // For double moves
    static constexpr bitboard row_2 = 0x000000000000ff00;
    static constexpr bitboard row_7 = 0x00ff000000000000;

    static constexpr bitboard side_edges = left_edge | right_edge;

    bitboard single_pawn_moves;
    bitboard double_pawn_moves;

    bitboard left_pawn_attacks;
    bitboard right_pawn_attacks;

    bitboard left_pawn_captures;
    bitboard right_pawn_captures;

    /// Generate moves for white pawns
    single_pawn_moves = (board.w_pawns << 8) & ~(board.w_pieces);
    // Convert to move array
    for (auto move : movgen::bitscan(single_pawn_moves & ~top_edge))
        moves->moves.push_back(Move(W_PAWN, move - 8, move));
    for (auto move : movgen::bitscan(single_pawn_moves& top_edge))
        moves->moves.push_back(Move(W_PAWN, move - 8, move, 0, 5));

    // Generate pawn attacks
    left_pawn_attacks = ((board.w_pawns & ~left_edge) << 9);
    right_pawn_attacks = ((board.w_pawns & ~right_edge) << 7);
    moves->w_pawn_attacks |= left_pawn_attacks;
    moves->w_pawn_attacks |= right_pawn_attacks;

    // Generete pawn captures
    left_pawn_captures = left_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));
    right_pawn_captures = right_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));

    for (auto move : movgen::bitscan(left_pawn_captures & ~top_edge))
        moves->moves.push_back(Move(W_PAWN, move - 9, move, get_piece(board, move, 2)));
    for (auto move : movgen::bitscan(right_pawn_captures & top_edge))
        moves->moves.push_back(Move(W_PAWN, move - 7, move, get_piece(board, move, 2), 5));

    /// All the same, but for black pawns

    single_pawn_moves = (board.w_pawns >> 8) & ~(board.b_pieces);
    // Convert to move array
    for (auto move : movgen::bitscan(single_pawn_moves & ~bottom_edge))
        moves->moves.push_back(Move(B_PAWN, move + 8, move));
    for (auto move : movgen::bitscan(single_pawn_moves& bottom_edge))
        moves->moves.push_back(Move(B_PAWN, move + 8, move, 0, 5));

    // Generate pawn attacks
    left_pawn_attacks = ((board.b_pawns & ~left_edge) >> 7);
    right_pawn_attacks = ((board.b_pawns & ~right_edge) >> 9);
    moves->b_pawn_attacks |= left_pawn_attacks;
    moves->b_pawn_attacks |= right_pawn_attacks;

    // Generete pawn captures
    left_pawn_captures = left_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));
    right_pawn_captures = right_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));

    for (auto move : movgen::bitscan(left_pawn_captures & ~bottom_edge))
        moves->moves.push_back(Move(B_PAWN, move + 7, move, get_piece(board, move, 1)));
    for (auto move : movgen::bitscan(right_pawn_captures & bottom_edge))
        moves->moves.push_back(Move(B_PAWN, move + 9, move, get_piece(board, move, 1), 5));
}