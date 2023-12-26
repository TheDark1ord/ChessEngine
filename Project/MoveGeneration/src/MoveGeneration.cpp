#include <iostream>
#include "../include/MoveGeneration.h"

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

void movgen::generate_move_cache(MoveCache* out)
{
    out = new MoveCache;

    ///Generate king moves
    //Moves relative to current position
    const int king_moves[]{ -9, -8, -7, -1, 1, 7, 8, 9 };
    for (int it = 0; it < 64; it++)
    {
        bitboard cur_board;
        for (int i = 0; i < 8; i++)
        {
            int move = it + king_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        out->king_cache[it] = cur_board;
    }

    ///Generate knight moves
    //Moves relative to current position
    const int knight_moves[]{ -17, -15, -10, -6, 6, 10, 15, 17 };
    for (int it = 0; it < 64; it++)
    {
        bitboard cur_board;
        for (int i = 0; i < 8; i++)
        {
            int move = it + knight_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        out->knight_cache[it] = cur_board;
    }

    movgen::generate_magics(out->magics);
}

movgen::BoardPosition movgen::board_from_fen(std::string fen)
{
    static std::regex fen_regex(movgen::fen_regex_string);
    if (!std::regex_match(fen, fen_regex))
    {
        throw std::runtime_error("Invalid fen string");
    }

    movgen::BoardPosition return_pos;
    int it = 0;

    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (isdigit(fen[it])) {
                j += static_cast<int>(fen[it] - '0');
                it++;

                if (j > 8)
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

                // This is forward slash(/) this shid does not work with char def
            case '/':
                if (j != 0) {
                    throw std::runtime_error("Invalid fen(while parsing piece information)");
                }
                j--;
                break;
            }
            it++;
        }
    }

    // Bitboard, representing 1st and last rows
    // set to 1
    bitboard top_and_bottom = 18374686479671623935ull;
    // Test that no pawns are on the first and last rows
    if (return_pos.w_pawns & top_and_bottom ||
        return_pos.b_pawns & top_and_bottom)
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
    // TODO: possible bug here(maybe should sub 1 from return_pos.en_passant)
    if ((1ull << return_pos.en_passant) & 18446463698227757054ull)
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
    throw std::runtime_error("Not implemented");
    return std::string();
}

void movgen::generateKingMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::MoveCache* cache)
{
    moves->w_king_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_kings))
        moves->w_king_attacks |= cache->king_cache[sq];
    moves->w_attacks |= moves->w_king_attacks;

    moves->b_king_attacks = 0;
    for (auto sq : movgen::bitscan(pos.b_kings))
        moves->b_king_attacks |= cache->king_cache[sq];
    moves->b_attacks |= moves->b_king_attacks;

    // This has to be generated two times, because we also should consider king attacks for each move
    for (auto sq : movgen::bitscan(pos.w_kings))
    {
        bitboard cur_moves = cache->knight_cache[sq];
        cur_moves &= !(pos.w_pieces | moves->b_attacks);
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_KING, sq, move, (int)get_piece(pos, move, 2)));
    }
    for (auto sq : movgen::bitscan(pos.b_kings))
    {
        bitboard cur_moves = cache->knight_cache[sq];
        cur_moves &= !(pos.b_pieces | moves->w_attacks);
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_KING, sq, move, (int)get_piece(pos, move, 1)));
    }
}

void movgen::generateQueenMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{
    moves->w_queen_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_queens))
    {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics) |
            movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->w_queen_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_QUEEN, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_queen_attacks;

    moves->b_queen_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_queens))
    {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics) |
            movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_queen_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_QUEEN, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_queen_attacks;
}

void movgen::generateRookMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{
    moves->w_rook_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_rooks))
    {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics);
        moves->w_rook_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_ROOK, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_rook_attacks;

    moves->b_rook_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_rooks))
    {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_rook_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_ROOK, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_rook_attacks;
}

void movgen::generateBishopMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::GeneratedMagics* magics)
{
    moves->w_bishop_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_bishops))
    {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->w_bishop_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_BISHOP, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_bishop_attacks;

    moves->b_bishop_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_bishops))
    {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_bishop_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_BISHOP, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_bishop_attacks;
}

void movgen::generateKnightMoves(movgen::BoardPosition& pos, movgen::GeneratedMoves* moves, movgen::MoveCache* cache)
{
    moves->w_knight_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_knights))
    {
        bitboard cur_moves = cache->knight_cache[sq];
        moves->w_knight_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_KNIGHT, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_knight_attacks;

    moves->b_knight_attacks = 0;
    for (auto sq : movgen::bitscan(pos.b_knights))
    {
        bitboard cur_moves = cache->knight_cache[sq];
        moves->b_knight_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_KNIGHT, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_knight_attacks;
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
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 8, move));
    for (auto move : movgen::bitscan(single_pawn_moves& top_edge))
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 8, move, 0, 5));

    // Generate pawn attacks
    left_pawn_attacks = ((board.w_pawns & ~left_edge) << 9);
    right_pawn_attacks = ((board.w_pawns & ~right_edge) << 7);
    moves->w_pawn_attacks |= left_pawn_attacks;
    moves->w_pawn_attacks |= right_pawn_attacks;

    // Generete pawn captures
    left_pawn_captures = left_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));
    right_pawn_captures = right_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));

    for (auto move : movgen::bitscan(left_pawn_captures & ~top_edge))
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 9, move, (int)get_piece(board, move, 2)));
    for (auto move : movgen::bitscan(right_pawn_captures& top_edge))
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 7, move, (int)get_piece(board, move, 2), 5));

    /// All the same, but for black pawns

    single_pawn_moves = (board.w_pawns >> 8) & ~(board.b_pieces);
    // Convert to move array
    for (auto move : movgen::bitscan(single_pawn_moves & ~bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 8, move));
    for (auto move : movgen::bitscan(single_pawn_moves& bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 8, move, 0, 5));

    // Generate pawn attacks
    left_pawn_attacks = ((board.b_pawns & ~left_edge) >> 7);
    right_pawn_attacks = ((board.b_pawns & ~right_edge) >> 9);
    moves->b_pawn_attacks |= left_pawn_attacks;
    moves->b_pawn_attacks |= right_pawn_attacks;

    // Generete pawn captures
    left_pawn_captures = left_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));
    right_pawn_captures = right_pawn_attacks & (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != -1));

    for (auto move : movgen::bitscan(left_pawn_captures & ~bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 7, move, (int)get_piece(board, move, 1)));
    for (auto move : movgen::bitscan(right_pawn_captures& bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 9, move, (int)get_piece(board, move, 1), 5));
}