#include "../include/MoveGeneration.h"
#include "../include/MagicNumbers.h"
#include <cassert>
#include <unordered_set>

bitboard movgen::knight_attacks[64];
bitboard movgen::king_attacks[64];

void movgen::init_movgen()
{
    /// Generate king moves
    // Moves relative to current position
    const int king_moves[]{-9, -8, -7, -1, 1, 7, 8, 9};
    for (int it = 0; it < 64; it++)
    {
        bitboard cur_board;
        for (int i = 0; i < 8; i++)
        {
            int move = it + king_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        king_attacks[it] = cur_board;
    }

    /// Generate knight moves
    // Moves relative to current position
    const int knight_moves[]{-17, -15, -10, -6, 6, 10, 15, 17};
    for (int it = 0; it < 64; it++)
    {
        bitboard cur_board;
        for (int i = 0; i < 8; i++)
        {
            int move = it + knight_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        knight_attacks[it] = cur_board;
    }
}

template <movgen::PieceType type>
void movgen::generate_piece_moves(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated)
{
    const unsigned int us = 8 * c;
    const unsigned int them = 8 - us;

    bitboard moves = get_pseudo_attacks<type>(piece_pos, pos.pieces[ALL_PIECES]) & ~pos.pieces[BLACK_PIECES + us];
    bitboard quiet = moves & ~pos.pieces[BLACK_PIECES + them];
    bitboard captures = moves & pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitb::bitscan(quiet))
        generated->push_back(Move(get_piece_from_type(type, c), piece_pos, move_to));
    for (auto move_to : bitb::bitscan(captures))
        generated->push_back(Move(get_piece_from_type(type, c), piece_pos, move_to, get_piece(pos, move_to)));
}

template void movgen::generate_piece_moves<movgen::QUEEN>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);
template void movgen::generate_piece_moves<movgen::ROOK>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);
template void movgen::generate_piece_moves<movgen::BISHOP>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);
template void movgen::generate_piece_moves<movgen::KNIGHT>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated);

template <>
void movgen::generate_piece_moves<movgen::KING>(bpos piece_pos, BoardPosition &pos, movgen::Color c, std::vector<Move> *generated)
{
    const unsigned int us = 8 * c;
    const unsigned int them = 8 - us;
    const auto short_castle = c == movgen::WHITE ? movgen::WHITE_SHORT : movgen::BLACK_SHORT;
    const auto long_castle = c == movgen::WHITE ? movgen::WHITE_LONG : movgen::BLACK_LONG;

    bitboard moves = get_pseudo_attacks<movgen::KING>(piece_pos, pos.pieces[ALL_PIECES]) & ~pos.pieces[BLACK_PIECES + us];
    bitboard quiet = moves & ~pos.pieces[BLACK_PIECES + them];
    bitboard captures = moves & pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitb::bitscan(quiet))
        generated->push_back(Move(get_piece_from_type(movgen::KING, c), piece_pos, move_to));
    for (auto move_to : bitb::bitscan(captures))
        generated->push_back(Move(get_piece_from_type(movgen::KING, c), piece_pos, move_to, get_piece(pos, move_to)));

    // Check for castling
    if (pos.castling_rights & short_castle)
        // Check if there is a rook
        if (pos.pieces[B_ROOK + us] & (bitb::sq_rank(piece_pos) & bitb::File[0]))
            generated->push_back(Move(get_piece_from_type(movgen::KING, c), piece_pos, piece_pos + 2, 0, 0, 0, 0, 1));

    if (pos.castling_rights & long_castle)
        // Check if there is a rook
        if (pos.pieces[B_ROOK + us] & (bitb::sq_rank(piece_pos) & bitb::File[7]))
            generated->push_back(Move(get_piece_from_type(movgen::KING, c), piece_pos, piece_pos - 2, 0, 0, 0, 0, 2));
}

template <movgen::Color c, movgen::GenType type>
void movgen::generate_pawn_moves(BoardPosition &pos, std::vector<Move> *generated)
{
    constexpr unsigned int us = 8 * c;
    constexpr unsigned int them = 8 - us;
    constexpr bitboard rank3 = (c == WHITE ? bitb::Rank[2] : bitb::Rank[5]);
    constexpr bitboard rank7 = (c == WHITE ? bitb::Rank[6] : bitb::Rank[1]);
    constexpr bitb::Direction advance = (c == WHITE ? bitb::UP : bitb::DOWN);
    constexpr Piece piece = (c == WHITE ? W_PAWN : B_PAWN);

    const bitboard prom_pawns = pos.pieces[B_PAWN + us] & rank7;
    const bitboard not_prom_pawns = pos.pieces[B_PAWN + us] & ~rank7;

    // Single and double moves
    bitboard s = shift<advance>(not_prom_pawns) & ~pos.pieces[BLACK_PIECES + them];
    bitboard d = shift<advance>(s & rank3) & ~pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitscan(s))
        generated->push_back(Move(piece, move_to - advance, move_to));
    for (auto move_to : bitscan(d))
        generated->push_back(Move(piece, move_to - advance * 2, move_to, 0, 0, true, false, 0));

    // Promotions
    if (prom_pawns)
    {
        bitboard left_capture = shift<advance + bitb::LEFT>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
        bitboard right_capture = shift<advance + bitb::RIGHT>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
        bitboard push = shift<advance>(prom_pawns) & ~pos.pieces[BLACK_PIECES + them];

        for (auto move_to : bitscan(left_capture))
            make_promotions<c, advance>(generated, move_to, get_piece(pos, move_to));
        for (auto move_to : bitscan(right_capture))
            make_promotions<c, advance>(generated, move_to, get_piece(pos, move_to));
        for (auto move_to : bitscan(push))
            make_promotions<c, advance>(generated, move_to, 0);
    }

    // Captures
    bitboard left_capture = shift<advance + bitb::LEFT>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];
    bitboard right_capture = shift<advance + bitb::RIGHT>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitscan(left_capture))
        generated->push_back(Move(piece, move_to - advance - bitb::LEFT, move_to, get_piece(pos, move_to)));
    for (auto move_to : bitscan(right_capture))
        generated->push_back(Move(piece, move_to - advance - bitb::RIGHT, move_to, get_piece(pos, move_to)));

    // En passant
    if (pos.en_passant != 0)
    {
        bitboard candidate_pawns = shift<-advance>(0x05 << pos.en_passant) & pos.pieces[B_PAWN + us];
        for (auto move_from : bitscan(candidate_pawns))
            generated->push_back(Move(piece, move_from, pos.en_passant,
                                      get_piece_from_type(movgen::PAWN, c), 0, false, true, 0));
    }
}

template <>
bitboard movgen::get_pseudo_attacks<movgen::KING>(bpos piece_pos, bitboard blocker)
{
    return king_attacks[piece_pos];
}

template <>
bitboard movgen::get_pseudo_attacks<movgen::QUEEN>(bpos piece_pos, bitboard blocker)
{
    return movgen::get_rook_attacks(blocker, piece_pos) |
           movgen::get_bishop_attacks(blocker, piece_pos);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, bitboard blocker)
{
    return movgen::get_rook_attacks(blocker, piece_pos);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, bitboard blocker)
{
    return movgen::get_bishop_attacks(blocker, piece_pos);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, bitboard blocker)
{
    return knight_attacks[piece_pos];
}

template <movgen::Color c>
void movgen::get_checkers(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * c;
    constexpr unsigned int them = 8 - us;
    constexpr bpos king_pos = pos.pieces[B_KING + us];

    bitboard king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES]);
    info->checkers |= king_sliding & (pos.pieces[B_BISHOP + them] | pos.pieces[B_QUEEN + them]);

    king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES]);
    info->checkers |= king_sliding & (pos.pieces[B_ROOK + them] | pos.pieces[B_QUEEN + them]);

    info->checks_num = bit_count(info->checkers);
}

template <movgen::Color c>
void movgen::get_pinners(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * c;
    constexpr unsigned int them = 8 - us;
    constexpr bpos king_pos = pos.pieces[B_KING + us];
    // Direction in which a pawn advances
    constexpr bitb::Direction advance = c == WHITE ? bitb::UP : bitb::DOWN;
    constexpr bitb::Direction retreat = c == WHITE ? bitb::DOWN : bitb::UP;

    bitboard king_sliding = get_pseudo_attacks<QUEEN>(king_pos, pos.pieces[ALL_PIECES]);
    bitboard candidates = king_sliding & pos.pieces[BLACK_PIECES + us];

    king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
    bitboard pinners = king_sliding & pos.pieces[BLACK_PIECES + them];
    for (bpos pin : bitscan(pinners))
    {
        if (get_piece(pos, pin, ~c) == (B_BISHOP + them) ||
            get_piece(pos, pin, ~c) == (B_QUEEN + them))
        {
            PositionInfo::Pin new_pin;

            new_pin.pinned = pop_lsb(pos.pieces[BLACK_PIECES + us] | bitb::Between_in[king_pos][pin]);
            new_pin.pinner = pin;
            new_pin.mask = bitb::Between[king_pos][new_pin.pinner];

            info->pins.push_back(new_pin);
            info->pin_board |= new_pin.pinned;
        }
    }

    king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
    pinners = king_sliding & pos.pieces[BLACK_PIECES + them];
    for (bpos pin : bitscan(pinners))
    {
        if (get_piece(pos, pin, ~c) == (B_ROOK + them) ||
            get_piece(pos, pin, ~c) == (B_QUEEN + them))
        {
            PositionInfo::Pin new_pin;

            new_pin.pinned = pos.pieces[BLACK_PIECES + us] | bitb::Line[king_pos][pin];
            new_pin.pinner = sq_bitb(pin);
            new_pin.mask = bitb::Between[king_pos][new_pin.pinner];

            info->pins.push_back(new_pin);
            info->pin_board |= new_pin.pinned;
        }
    }

    // It is possible that this code detects a check instead of a pin, but in that case we would not be able to do en passant anyway
    // Check for en passant pin
    if (pos.en_passant != 0)
    {
        // If the king is not on the same rank as the pawns, en passant pin is impossible
        if (shift<advance>(sq_rank(king_pos)) == sq_rank(pos.en_passant))
        {
            bitboard left_pawn = shift<retreat + bitb::LEFT>(sq_bitb(pos.en_passant));
            if (left_pawn & pos.pieces[B_PAWN + us])
            {
                bitboard wo_left = pos.pieces[ALL_PIECES] ^ left_pawn;
                bitboard pinner_mask = movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_left) & sq_rank(king_pos);
                if (pinner_mask & (pos.pieces[B_QUEEN + them] | pos.pieces[B_ROOK + them]))
                    info->en_passant_pin = 0;
            }

            bitboard right_pawn = shift<retreat + bitb::RIGHT>(sq_bitb(pos.en_passant));
            if (&pos.pieces[B_PAWN + us])
            {
                bitboard wo_right = pos.pieces[ALL_PIECES] ^ right_pawn;
                bitboard pinner_mask = movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_right) & sq_rank(king_pos);
                if (pinner_mask & (pos.pieces[B_QUEEN + them] | pos.pieces[B_ROOK + them]))
                    info->en_passant_pin = 0;
            }
        }
    }
}

template <movgen::Color c>
void movgen::get_attacked(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * c;
    constexpr bitb::Direction advance = c == WHITE ? bitb::UP : bitb::DOWN;

    auto get_attacks = [&](Piece piece)
    {
        bitboard attacks = 0;
        for (bpos p_pos : bitscan(pos.pieces[piece]))
            attacks |= get_pseudo_attacks<piece>(p_pos, pos.pieces[ALL_PIECES]);
        return attacks;
    };
    info->attacked |= get_attacks(B_KING + us);
    info->attacked |= get_attacks(B_QUEEN + us);
    info->attacked |= get_attacks(B_ROOK + us);
    info->attacked |= get_attacks(B_BISHOP + us);
    info->attacked |= get_attacks(B_KNIGHT + us);
    info->attacked |= shift<advance + bitb::LEFT>(pos.pieces[B_PAWN + us]) | shift<advance + bitb::RIGHT>(pos.pieces[B_PAWN + us]);
}

template <movgen::Color c>
std::vector<movgen::Move> *movgen::generate_all_moves(BoardPosition &pos)
{
    std::vector<movgen::Move> *moves = new std::vector<movgen::Move>;
    // TODO: Test optimal reserve number
    moves->reserve(10);

    auto gen_moves = [&](movgen::PieceType piece_type)
    {
        const movgen::Piece piece = get_piece_from_type(piece_type, c);

        for (auto piece_pos : bitscan(pos.pieces[piece]))
            generate_piece_moves<piece_type>(piece_pos, pos, c, moves);
    };

    gen_moves(movgen::KING);
    gen_moves(movgen::QUEEN);
    gen_moves(movgen::ROOK);
    gen_moves(movgen::BISHOP);
    gen_moves(movgen::KNIGHT);
    movgen::generate_pawn_moves(pos, moves);

    return moves;
}

std::vector<movgen::Move> movgen::get_legal_moves(BoardPosition &pos, std::vector<Move> &generated)
{
    const Color c = pos.side_to_move;
    const unsigned int us = 8 * c;
    const bitboard pinned = pos.info->pin_board;
    const bpos ksq = pos.pieces[B_KING + us];

    std::vector<Move> legal_moves;
    legal_moves.reserve(generated.size());

    // Only king moves are possible
    if (pos.info->checks_num >= 2)
    {
        for (Move move : generated)
        {
            if (move.from == ksq && get_move_type(move) != CASTLING)
            {
                if (!(pos.info->attacked & bitb::sq_bitb(move.to)))
                {
                    legal_moves.push_back(move);
                }
            }
        }
        return legal_moves;
    }
    // Only allow king moves and blockers
    else if (pos.info->checks_num == 1)
    {
        for (Move move : generated)
        {
            if (move.to & pos.info->blockers ||
                (move.from == ksq && get_move_type(move) != CASTLING && is_legal(pos, move)))
            {
                legal_moves.push_back(move);
            }
        }
        return legal_moves;
    }

    for (Move move : generated)
    {
        // Check for legality only if one these 4 requirements is met
        if (!(bitb::sq_bitb(move.from) & pinned || move.from == ksq || get_move_type(move) == EN_PASSANT) ||
            is_legal(pos, move))
            legal_moves.push_back(move);
    }
    return legal_moves;
}

void movgen::make_move(movgen::BoardPosition *pos, movgen::Move &move, std::unordered_set<movgen::BoardHash> *hashed_positions)
{
    const bitb::Direction down = pos->side_to_move == WHITE ? bitb::DOWN : bitb::UP;
    const movgen::Color cur_color = pos->side_to_move;

    // Flip the color
    pos->side_to_move = static_cast<movgen::Color>(~pos->side_to_move);
    if (pos->side_to_move == movgen::WHITE)
        pos->fullmove++;
    pos->halfmove++; // Reset later, if necessary
    pos->en_passant = 0;

    switch (move.get_type())
    {
    case CAPTURE:
        pos->pieces[pos->squares[move.to]] &= ~(1ull << move.to);
    // Fallthrough
    case REGULAR:
        move_piece(pos, move.piece, move.from, move.to);
        break;
    case PROMOTION_CAPTURE:
        pos->pieces[pos->squares[move.to]] &= ~(1ull << move.to);
    // Fallthrough
    case PROMOTION:
    {
        const char prom = (move.move_data << 4) & 0xF;
        assert(2 <= prom && prom < 6);
        const movgen::Piece new_piece = get_piece_from_type(
            static_cast<movgen::PieceType>(prom), cur_color);

        pos->pieces[move.piece] &= ~(1ull << move.from);
        pos->pieces[new_piece] |= (1ull << move.to);

        pos->squares[move.from] = movgen::NO_PIECE;
        pos->squares[move.to] = new_piece;
        break;
    }
    case EN_PASSANT:
        pos->pieces[pos->squares[move.to]] &= ~(1ull << (move.to + down));
        move_piece(pos, move.piece, move.from, move.to);
        break;
    case DOUBLE_MOVE:
        pos->en_passant = move.to + down;
        move_piece(pos, move.piece, move.from, move.to);
        break;
    case CASTLING:
    {
        const bitboard king_rank = bitb::sq_rank(move.from);
        const movgen::Piece rook_piece = get_piece_from_type(movgen::ROOK, cur_color);

        // Short castling
        if (((move.move_data << 10) & 0x4) == 1)
        {
            pos->pieces[move.piece] &= 1ull << move.from;
            pos->pieces[move.piece] |= king_rank & bitb::File[6];
            pos->squares[move.from] = movgen::NO_PIECE;
            pos->squares[move.from + 2] = move.piece;

            pos->pieces[rook_piece] &= 1ull << (move.from + 3);
            pos->pieces[rook_piece] |= king_rank & bitb::File[5];
            pos->squares[move.from + 3] = movgen::NO_PIECE;
            pos->squares[move.from + 1] = rook_piece;
        }
        else
        {
            pos->pieces[move.piece] &= 1ull << move.from;
            pos->pieces[move.piece] |= king_rank & bitb::File[2];
            pos->squares[move.from] = movgen::NO_PIECE;
            pos->squares[move.from + 2] = move.piece;

            pos->pieces[rook_piece] &= 1ull << (move.from - 4);
            pos->pieces[rook_piece] |= king_rank & bitb::File[3];
            pos->squares[move.from - 4] = movgen::NO_PIECE;
            pos->squares[move.from - 1] = rook_piece;
        }

        pos->castling_rights ^= cur_color == WHITE ? movgen::WHITE_CASTLE : BLACK_CASTLE;
        break;
    }
    }

    if (movgen::get_piece_type(move.piece) == movgen::KING)
        pos->castling_rights = movgen::NO_CASTLING;
    else if (movgen::get_piece_type(move.piece) == movgen::ROOK)
    {
        // a rank rook
        if (pos->pieces[move.piece] & bitb::File[0])
            pos->castling_rights ^= cur_color == WHITE ? movgen::WHITE_LONG : BLACK_LONG;
        // h rank rook
        else if (pos->pieces[move.piece] & bitb::File[7])
            pos->castling_rights ^= cur_color == WHITE ? movgen::WHITE_SHORT : BLACK_SHORT;
    }

    auto cur_hash = movgen::BoardHash(*pos);
    if (auto prev = hashed_positions->find(cur_hash); prev != hashed_positions->end())
        prev->reached++;
    else
        hashed_positions->insert(cur_hash);
}

bool is_legal(movgen::BoardPosition &pos, movgen::Move move)
{
    const movgen::Color c = pos.side_to_move;
    const bpos from = move.from;
    const bpos to = move.to;

    if (get_move_type(move) == movgen::EN_PASSANT)
    {
        return !(pos.info->en_passant_pin);
    }
    else if (get_move_type(move) == movgen::CASTLING)
    {
        // Check that no square inbetween are under attack
        bitboard rook_sq = from > to ? from - 3 : from + 4;
        return !(pos.info->attacked & bitb::Between_in[from][rook_sq]);
    }
    else if (get_piece_type(movgen::get_piece(pos, move.from)) == movgen::KING)
    {
        return !(pos.info->attacked & bitb::sq_bitb(to));
    }

    // A pinned piece can move only to specified squares
    for (auto pin : pos.info->pins)
        if (pin.pinned == move.from)
            return bitb::sq_bitb(move.to) & pin.mask;
    return true;
}

void bitb_movearray(movgen::Piece piece, bpos starting_pos, bitboard move_board, bitboard them, movgen::BoardPosition &pos, std::vector<movgen::Move> *move_arr)
{
    bitboard captures = move_board & them;
    bitboard quiet = move_board ^ captures;

    for (auto pos_to : bitb::bitscan(quiet))
        move_arr->push_back(movgen::Move(piece, starting_pos, pos_to));
    for (auto pos_to : bitb::bitscan(captures))
        move_arr->push_back(movgen::Move(piece, starting_pos, pos_to, movgen::get_piece(pos, pos_to)));
}

template <movgen::Color c, bitb::Direction d>
inline void make_promotions(std::vector<movgen::Move> *move_arr, bpos to, unsigned char capture)
{
    constexpr movgen::Piece piece = (c == movgen::WHITE ? movgen::W_PAWN : movgen::B_PAWN);

    move_arr->push_back(Move(piece, to - d, to, capture, movgen::QUEEN, false, false, 0));
    move_arr->push_back(Move(piece, to - d, to, capture, movgen::ROOK, false, false, 0));
    move_arr->push_back(Move(piece, to - d, to, capture, movgen::BISHOP, false, false, 0));
    move_arr->push_back(Move(piece, to - d, to, capture, movgen::KNIGHT, false, false, 0));
}

void move_piece(movgen::BoardPosition *pos, movgen::Piece piece, bpos from, bpos to)
{
    pos->pieces[piece] &= ~(1ull << from);
    pos->pieces[piece] |= (1ull << to);

    pos->squares[from] = movgen::NO_PIECE;
    pos->squares[to] = piece;
}
