#include "../include/MoveGeneration.h"
#include "../include/MagicNumbers.h"
#include <cassert>
#include <unordered_set>

bitboard movgen::knight_attacks[64];
bitboard movgen::king_attacks[64];
std::atomic_bool movgen::initialized = false;

void movgen::init()
{
    const std::tuple<int, int> king_moves[]{
        {-1, -1},
        {0, -1},
        {1, -1},
        {-1, 0},
        {1, 0},
        {-1, 1},
        {0, 1},
        {1, 1}};

    const std::tuple<int, int> knight_moves[]{
        {-1, -2},
        {1, -2},
        {-2, -1},
        {2, -1},
        {-1, 2},
        {1, 2},
        {-2, 1},
        {2, 1},
    };

    for (uint16_t y = 0; y < 8; y++)
    {
        for (uint16_t x = 0; x < 8; x++)
        {
            bitboard king_board = 0;
            bitboard knight_board = 0;

            for (auto offset : king_moves)
            {
                int ky = y + std::get<0>(offset);
                int kx = x + std::get<1>(offset);

                if (ky >= 0 && ky < 8 &&
                    kx >= 0 && kx < 8)
                    king_board |= 1ull << (ky * 8 + kx);
            }
            for (auto offset : knight_moves)
            {
                int ky = y + std::get<0>(offset);
                int kx = x + std::get<1>(offset);

                if (ky >= 0 && ky < 8 &&
                    kx >= 0 && kx < 8)
                    knight_board |= 1ull << (ky * 8 + kx);
            }

            king_attacks[y * 8 + x] = king_board;
            knight_attacks[y * 8 + x] = knight_board;
        }
    }

    initialized = true;
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

template <movgen::Color color>
void movgen::generate_pawn_moves(BoardPosition &pos, std::vector<Move> *generated)
{
    constexpr unsigned int us = 8 * color;
    constexpr unsigned int them = 8 - us;
    constexpr bitboard rank3 = (color == WHITE ? bitb::Rank[2] : bitb::Rank[5]);
    constexpr bitboard rank7 = (color == WHITE ? bitb::Rank[6] : bitb::Rank[1]);
    constexpr Piece piece = (color == WHITE ? W_PAWN : B_PAWN);

    constexpr bitb::Direction forward = (color == WHITE ? bitb::UP : bitb::DOWN);
    constexpr bitb::Direction back = (color == WHITE ? bitb::DOWN : bitb::UP);
    constexpr bitb::Direction forward_left = static_cast<bitb::Direction>(forward + bitb::LEFT);
    constexpr bitb::Direction forward_right = static_cast<bitb::Direction>(forward + bitb::RIGHT);

    const bitboard prom_pawns = pos.pieces[B_PAWN + us] & rank7;
    const bitboard not_prom_pawns = pos.pieces[B_PAWN + us] & ~rank7;

    // Single and double moves
    bitboard s = bitb::shift<forward>(not_prom_pawns) & ~pos.pieces[BLACK_PIECES + them];
    bitboard d = bitb::shift<forward>(s & rank3) & ~pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitb::bitscan(s))
        generated->push_back(Move(piece, move_to - forward, move_to));
    for (auto move_to : bitb::bitscan(d))
        generated->push_back(Move(piece, move_to - forward * 2, move_to, 0, 0, true, false, 0));

    // Promotions
    if (prom_pawns)
    {
        bitboard left_capture = bitb::shift<forward_left>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
        bitboard right_capture = bitb::shift<forward_right>(prom_pawns) & pos.pieces[BLACK_PIECES + them];
        bitboard push = bitb::shift<forward>(prom_pawns) & ~pos.pieces[BLACK_PIECES + them];

        for (auto move_to : bitb::bitscan(left_capture))
            make_promotions<color, forward>(generated, move_to, get_piece(pos, move_to));
        for (auto move_to : bitb::bitscan(right_capture))
            make_promotions<color, forward>(generated, move_to, get_piece(pos, move_to));
        for (auto move_to : bitb::bitscan(push))
            make_promotions<color, forward>(generated, move_to, 0);
    }

    // Captures
    bitboard left_capture = bitb::shift<forward_left>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];
    bitboard right_capture = bitb::shift<forward_right>(not_prom_pawns) & pos.pieces[BLACK_PIECES + them];

    for (auto move_to : bitb::bitscan(left_capture))
        generated->push_back(Move(piece, move_to - forward - bitb::LEFT, move_to, get_piece(pos, move_to)));
    for (auto move_to : bitb::bitscan(right_capture))
        generated->push_back(Move(piece, move_to - forward - bitb::RIGHT, move_to, get_piece(pos, move_to)));

    // En passant
    if (pos.en_passant != 0)
    {
        bitboard candidate_pawns = bitb::shift<back>(0x05 << pos.en_passant) & pos.pieces[B_PAWN + us];
        for (auto move_from : bitb::bitscan(candidate_pawns))
            generated->push_back(Move(piece, move_from, pos.en_passant,
                                      get_piece_from_type(movgen::PAWN, color), 0, false, true, 0));
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
    return movgen::get_rook_attacks(piece_pos, blocker) |
           movgen::get_bishop_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::ROOK>(bpos piece_pos, bitboard blocker)
{
    return movgen::get_rook_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::BISHOP>(bpos piece_pos, bitboard blocker)
{
    return movgen::get_bishop_attacks(piece_pos, blocker);
}
template <>
bitboard movgen::get_pseudo_attacks<movgen::KNIGHT>(bpos piece_pos, bitboard blocker)
{
    return knight_attacks[piece_pos];
}

template <movgen::Color color>
void movgen::get_checkers(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * color;
    constexpr unsigned int them = 8 - us;
    const bpos king_pos = pos.pieces[B_KING + them];

    bitboard king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES]);
    info->checkers |= king_sliding & (pos.pieces[B_BISHOP + us] | pos.pieces[B_QUEEN + us]);

    king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES]);
    info->checkers |= king_sliding & (pos.pieces[B_ROOK + us] | pos.pieces[B_QUEEN + us]);

    info->checks_num = bitb::bit_count(info->checkers);
}

template <movgen::Color c>
void movgen::get_pinners(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * c;
    constexpr unsigned int them = 8 - us;
    // Direction in which a pawn advances
    constexpr bitb::Direction forward = c == WHITE ? bitb::UP : bitb::DOWN;
    constexpr bitb::Direction back = c == WHITE ? bitb::DOWN : bitb::UP;
    constexpr bitb::Direction back_left = static_cast<bitb::Direction>(back + bitb::LEFT);
    constexpr bitb::Direction back_right = static_cast<bitb::Direction>(back + bitb::RIGHT);

    bpos king_pos = bitb::pop_lsb(pos.pieces[B_KING + them]);

    bitboard king_sliding = get_pseudo_attacks<QUEEN>(king_pos, pos.pieces[ALL_PIECES]);
    bitboard candidates = king_sliding & pos.pieces[BLACK_PIECES + them];

    king_sliding = get_pseudo_attacks<BISHOP>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
    bitboard pinners = king_sliding & pos.pieces[BLACK_PIECES + us];
    for (bpos pin : bitb::bitscan(pinners))
    {
        if (get_piece(pos, pin) == (B_BISHOP + us) ||
            get_piece(pos, pin) == (B_QUEEN + us))
        {
            PositionInfo::Pin new_pin;

            new_pin.pinned = bitb::pop_lsb(pos.pieces[BLACK_PIECES + them] | bitb::Between_in[king_pos][pin]);
            new_pin.pinner = pin;
            new_pin.mask = bitb::Between[king_pos][new_pin.pinner];

            info->pins.push_back(new_pin);
            info->pin_board |= new_pin.pinned;
        }
    }

    king_sliding = get_pseudo_attacks<ROOK>(king_pos, pos.pieces[ALL_PIECES] ^ candidates);
    pinners = king_sliding & pos.pieces[BLACK_PIECES + us];
    for (bpos pin : bitb::bitscan(pinners))
    {
        if (get_piece(pos, pin) == (B_ROOK + us) ||
            get_piece(pos, pin) == (B_QUEEN + us))
        {
            PositionInfo::Pin new_pin;

            new_pin.pinned = pos.pieces[BLACK_PIECES + them] | bitb::Line[king_pos][pin];
            new_pin.pinner = bitb::sq_bitb(pin);
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
        if (bitb::shift<forward>(bitb::sq_rank(king_pos)) == bitb::sq_rank(pos.en_passant))
        {
            bitboard left_pawn = bitb::shift<back_left>(bitb::sq_bitb(pos.en_passant));
            if (left_pawn & pos.pieces[B_PAWN + them])
            {
                bitboard wo_left = pos.pieces[ALL_PIECES] ^ left_pawn;
                bitboard pinner_mask = movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_left) & bitb::sq_rank(king_pos);
                if (pinner_mask & (pos.pieces[B_QUEEN + us] | pos.pieces[B_ROOK + us]))
                    info->en_passant_pin = 0;
            }

            bitboard right_pawn = bitb::shift<back_right>(bitb::sq_bitb(pos.en_passant));
            if (&pos.pieces[B_PAWN + them])
            {
                bitboard wo_right = pos.pieces[ALL_PIECES] ^ right_pawn;
                bitboard pinner_mask = movgen::get_pseudo_attacks<movgen::PieceType::ROOK>(king_pos, wo_right) & bitb::sq_rank(king_pos);
                if (pinner_mask & (pos.pieces[B_QUEEN + us] | pos.pieces[B_ROOK + us]))
                    info->en_passant_pin = 0;
            }
        }
    }
}

template <movgen::Color color>
void movgen::get_attacked(BoardPosition pos, PositionInfo *info)
{
    constexpr unsigned int us = 8 * color;
    constexpr bitb::Direction forward = color == WHITE ? bitb::UP : bitb::DOWN;
    constexpr bitb::Direction forward_left = static_cast<bitb::Direction>(forward + bitb::LEFT);
    constexpr bitb::Direction forward_right = static_cast<bitb::Direction>(forward + bitb::RIGHT);

#define get_attacks(attacks, piece)                     \
    for (bpos p_pos : bitb::bitscan(pos.pieces[piece + us])) \
        attacks |= get_pseudo_attacks<piece>(p_pos, pos.pieces[ALL_PIECES]);

    get_attacks(info->attacked, KING);
    get_attacks(info->attacked, QUEEN);
    get_attacks(info->attacked, ROOK);
    get_attacks(info->attacked, BISHOP);
    get_attacks(info->attacked, KNIGHT);
    info->attacked |= bitb::shift<forward_left>(pos.pieces[B_PAWN + us]) | bitb::shift<forward_right>(pos.pieces[B_PAWN + us]);
}

template <movgen::Color color>
std::vector<movgen::Move> *movgen::generate_all_moves(BoardPosition &pos)
{
    constexpr movgen::Color col_them = movgen::Color(!color);
    std::vector<movgen::Move> *moves = new std::vector<movgen::Move>;
    // TODO: Test optimal reserve number
    moves->reserve(10);

    for (auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::KING, color)]))
        generate_piece_moves<movgen::KING>(piece_pos, pos, color, moves);
    for (auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::QUEEN, color)]))
        generate_piece_moves<movgen::QUEEN>(piece_pos, pos, color, moves);
    for (auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::ROOK, color)]))
        generate_piece_moves<movgen::ROOK>(piece_pos, pos, color, moves);
    for (auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::BISHOP, color)]))
        generate_piece_moves<movgen::BISHOP>(piece_pos, pos, color, moves);
    for (auto piece_pos : bitb::bitscan(pos.pieces[get_piece_from_type(movgen::KNIGHT, color)]))
        generate_piece_moves<movgen::KNIGHT>(piece_pos, pos, color, moves);

    movgen::generate_pawn_moves<color>(pos, moves);

    pos.info = new PositionInfo;

    get_checkers<col_them>(pos, pos.info);
    get_pinners<col_them>(pos, pos.info);
    get_attacked<col_them>(pos, pos.info);

    return moves;
}

template std::vector<movgen::Move> *movgen::generate_all_moves<movgen::WHITE>(BoardPosition &pos);
template std::vector<movgen::Move> *movgen::generate_all_moves<movgen::BLACK>(BoardPosition &pos);

std::vector<movgen::Move> *movgen::get_legal_moves(BoardPosition &pos, std::vector<Move> &generated)
{
    const Color c = pos.side_to_move;
    const unsigned int us = 8 * c;
    const bitboard pinned = pos.info->pin_board;
    const bpos ksq = pos.pieces[B_KING + us];

    std::vector<Move> *legal_moves = new std::vector<Move>;
    legal_moves->reserve(generated.size());

    // Only king moves are possible
    if (pos.info->checks_num >= 2)
    {
        for (Move move : generated)
        {
            if (move.from == ksq && move.get_type() != CASTLING)
            {
                if (!(pos.info->attacked & bitb::sq_bitb(move.to)))
                {
                    legal_moves->push_back(move);
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
                (move.from == ksq && move.get_type() != CASTLING && is_legal(pos, move)))
            {
                legal_moves->push_back(move);
            }
        }
        return legal_moves;
    }

    for (Move move : generated)
    {
        // Check for legality only if one these 4 requirements is met
        if (!(bitb::sq_bitb(move.from) & pinned || move.from == ksq || move.get_type() == EN_PASSANT) ||
            is_legal(pos, move))
            legal_moves->push_back(move);
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

    // Assign composite bitboards
    pos->pieces[BLACK_PIECES] = pos->pieces[B_KING] | pos->pieces[B_QUEEN] |
                                pos->pieces[B_ROOK] | pos->pieces[B_BISHOP] |
                                pos->pieces[B_KNIGHT] | pos->pieces[B_PAWN];
    pos->pieces[WHITE_PIECES] = pos->pieces[W_KING] | pos->pieces[W_QUEEN] |
                                pos->pieces[W_ROOK] | pos->pieces[W_BISHOP] |
                                pos->pieces[W_KNIGHT] | pos->pieces[W_PAWN];
    pos->pieces[ALL_PIECES] = pos->pieces[BLACK_PIECES] | pos->pieces[WHITE_PIECES];

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

    if (move.get_type() == movgen::EN_PASSANT)
    {
        return !(pos.info->en_passant_pin);
    }
    else if (move.get_type() == movgen::CASTLING)
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

    move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::QUEEN, false, false, 0));
    move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::ROOK, false, false, 0));
    move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::BISHOP, false, false, 0));
    move_arr->push_back(movgen::Move(piece, to - d, to, capture, movgen::KNIGHT, false, false, 0));
}

void move_piece(movgen::BoardPosition *pos, movgen::Piece piece, bpos from, bpos to)
{
    pos->pieces[piece] &= ~(1ull << from);
    pos->pieces[piece] |= (1ull << to);

    pos->squares[from] = movgen::NO_PIECE;
    pos->squares[to] = piece;
}
