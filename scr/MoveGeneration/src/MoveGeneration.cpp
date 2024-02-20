#include "../include/MoveGeneration.h"

bitboard movgen::knight_attacks[64];
bitboard movgen::king_attacks[64];

size_t std::hash<movgen::BoardPosition>::operator()(
    movgen::BoardPosition const& p) const noexcept {
    // TODO: board hash
    size_t hash = 0;
    return size_t();
}

movgen::BoardHash::BoardHash(BoardPosition& pos) {
    this->hash = std::hash<movgen::BoardPosition>{}(pos);
}

size_t movgen::BoardHash::operator()(BoardHash const& h) const {
    return this->hash;
}

void movgen::init_movgen() {
    /// Generate king moves
    // Moves relative to current position
    const int king_moves[]{ -9, -8, -7, -1, 1, 7, 8, 9 };
    for (int it = 0; it < 64; it++) {
        bitboard cur_board;
        for (int i = 0; i < 8; i++) {
            int move = it + king_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        king_attacks[it] = cur_board;
    }

    /// Generate knight moves
    // Moves relative to current position
    const int knight_moves[]{ -17, -15, -10, -6, 6, 10, 15, 17 };
    for (int it = 0; it < 64; it++) {
        bitboard cur_board;
        for (int i = 0; i < 8; i++) {
            int move = it + knight_moves[i];
            if (move >= 0 && move < 64)
                cur_board |= 1ull << move;
        }
        knight_attacks[it] = cur_board;
    }
}

movgen::BoardPosition movgen::board_from_fen(std::string fen) {
    static std::regex fen_regex(movgen::fen_regex_string);
    if (!std::regex_match(fen, fen_regex)) {
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

            switch (fen[it]) {
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
                if (j != 0) {
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
    // set to 1
    bitboard top_and_bottom = 18374686479671623935ull;
    // Test that no pawns are on the first and last rows
    if (return_pos.w_pawns & top_and_bottom ||
        return_pos.b_pawns & top_and_bottom) {
        throw std::runtime_error("Invalid fen");
    }

    // Assign composite bitboards
    return_pos.b_pieces = return_pos.b_kings | return_pos.b_queens |
        return_pos.b_rooks | return_pos.b_bishops |
        return_pos.b_knights | return_pos.b_pawns;
    return_pos.w_pieces = return_pos.w_kings | return_pos.w_queens |
        return_pos.w_rooks | return_pos.w_bishops |
        return_pos.w_knights | return_pos.w_pawns;
    return_pos.all_pieces = return_pos.b_pieces | return_pos.w_pieces;

    // Scip spaces
    while (fen[++it] == ' ') {
    };
    return_pos.side = (movgen::Color)(fen[it] == 'w');

    // Scip spaces
    while (fen[++it] == ' ') {
    }
    while (fen[it] != ' ') {
        switch (fen[it++]) {
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
    while (fen[++it] == ' ') {
    }
    if (fen[it] == '-') {
        // It is a valid position, but considering, that with correct position
        // a pawn cannot take that en passant
        return_pos.en_passant = 0;
    }
    else {
        return_pos.en_passant = (fen[it] - 'a') + (fen[++it] - '1') * 8;
    }
    // Test that en passant square is on 3rd or 6th row
    // TODO: possible bug here(maybe should sub 1 from return_pos.en_passant)
    if ((1ull << return_pos.en_passant) & 18446463698227757054ull) {
        throw std::runtime_error("Invalid fen");
    }

    while (fen[++it] == ' ') {
    }
    return_pos.halfmove = fen[it] - '0';
    while (fen[++it] == ' ') {
    }
    return_pos.fullmove = fen[it] - '0';

    return return_pos;
}

std::string movgen::board_to_fen(movgen::BoardPosition& pos) {
    // TODO: board_to_fen
    throw std::runtime_error("Not implemented");
    return std::string();
}

// Generate legal moves
movgen::GeneratedMoves movgen::generate_moves(BoardPosition pos,
    std::set<size_t>* hashed_pos) {
    movgen::GeneratedMoves legal_moves;

    // Number of piecesm that are currently giving a check
    unsigned int attackers = 0;
    // Which squares we can move to to block the check or capture the attaking
    // piece
    bitboard blocker_mask = 0;
    // Attacked squares
    bitboard attacked = 0;
    bpos king_pos;

    bitboard king_attacks, queen_attacks, rook_attacks, bishop_attacks,
        knight_attacks, pawn_attacks;

    // Process attacks for a piece
    auto piece_attacks =
        [&](bitboard piece_pos, bitboard board, bool is_sliding_piece,
            bpos king_pos, movgen::PieceType piece)
        [[msvc::forceinline]] {
        // First step is to generate attacks for an opposite side
        bitboard attacks = 0;
        for (auto piece : movgen::bitscan(piece_pos)) {
            bitboard piece_attacks = movgen::generate_piece_moves<piece>(piece, board, hashed_moves);

            // For each piece check whether it is attacking the king or not
            if (static_cast<bool>(king_pos & piece_attacks)) {
                attackers++;
                // If this is a sliding piece, we can evade the check by
                // blocking it or capturing the attaking piece
                if (is_sliding_piece)
                    blocker_mask |= movgen::slider_rays_to_square(king_pos, piece_pos);
                // Else, only capture
                else
                    blocker_mask |= piece_pos;
            }
            attacks |= piece_attacks;
        }
        return attacks;
        };

    // White to move
    if (static_cast<unsigned int>(pos.side)) {
        // Remove white king from the bitboard before generating attacks
        //  This as done to check for the king moving away from the sliding attacker
        bitboard pieces = pos.all_pieces & ~pos.w_kings;
        king_pos = movgen::bitscan(pos.w_kings)[0];

        queen_attacks = piece_attacks(pos.b_queens, pieces, true, king_pos, movgen::PieceType::QUEEN);
        rook_attacks = piece_attacks(pos.b_rooks, pieces, true, king_pos, movgen::PieceType::ROOK);
        bishop_attacks = piece_attacks(pos.b_bishops, pieces, true, king_pos, movgen::PieceType::BISHOP);
        knight_attacks = piece_attacks(pos.b_knights, pieces, true, king_pos, movgen::PieceType::KNIGHT);
        king_attacks = movgen::generate_piece_moves(movgen::bitscan(pos.b_kings)[0], pos.all_pieces, hashed_moves);

        // Pawn move generation is a bit different because of en passant
        pawn_attacks = pos.b_pawns << 8;
        if (pawn_attacks & king_pos) {
            if ((king_pos >> 7) & pos.b_pawns) {
                // Edit blocker mask to include en passant
                if (pos.en_passant > 0 && (1ull << pos.en_passant) & (king_pos >> 15))
                    blocker_mask |= 1ull << pos.en_passant;
                blocker_mask |= (king_pos >> 7);
            }
            if ((king_pos >> 9) & pos.b_pawns) {
                // Edit blocker mask to include en passant
                if (pos.en_passant > 0 && (1ull << pos.en_passant) & (king_pos >> 17))
                    blocker_mask |= 1ull << pos.en_passant;
                blocker_mask |= (king_pos >> 9);
            }
        }

        // If the king is on the same rank as en passant pawn it is possible to have a special en passant pin
        if (pos.en_passant > 0) {
            bitboard king_rank = get_hline(king_pos);
            bitboard en_passant_pawn = 1ull << (pos.en_passant + 8);
            if (king_rank & en_passant_pawn) {
                if (pos.b_pieces & (en_passant_pawn << 1))
                {
                    bitboard wo_left = pos.all_pieces & !(en_passant_pawn | (en_passant_pawn << 1));
                    bitboard pinner_mask = movgen::generate_piece_moves<movgen::PieceType::ROOK>(king_pos, wo_left, hashed_moves) & king_rank;
                    if (pinner_mask & (pos.b_queens | pos.b_rooks))
                        pos.en_passant = 0;
                }
                if (pos.b_pieces & (en_passant_pawn >> 1))
                {
                    bitboard wo_left = pos.all_pieces & !(en_passant_pawn | (en_passant_pawn >> 1));
                    bitboard pinner_mask = movgen::generate_piece_moves<movgen::PieceType::ROOK>(king_pos, wo_left, hashed_moves) & king_rank;
                    if (pinner_mask & (pos.b_queens | pos.b_rooks))
                        pos.en_passant = 0;
                }
            }
        }
    }
    // Black to move
    else {
        bitboard pieces = pos.all_pieces & ~pos.b_kings;
        king_pos = movgen::bitscan(pos.b_kings)[0];

        queen_attacks = piece_attacks(pos.b_queens, pieces, true, king_pos, movgen::PieceType::QUEEN);
        rook_attacks = piece_attacks(pos.b_rooks, pieces, true, king_pos, movgen::PieceType::ROOK);
        bishop_attacks = piece_attacks(pos.b_bishops, pieces, true, king_pos, movgen::PieceType::BISHOP);
        knight_attacks = piece_attacks(pos.b_knights, pieces, true, king_pos, movgen::PieceType::KNIGHT);
        king_attacks = movgen::generate_piece_moves(movgen::bitscan(pos.w_kings)[0], pos.all_pieces, hashed_moves);

        pawn_attacks = pos.w_pawns >> 8;
        if (pawn_attacks & king_pos) {
            if ((king_pos << 7) & pos.b_pawns) {
                if (pos.en_passant > 0 && (1ull << pos.en_passant) & (king_pos << 15))
                    blocker_mask |= 1ull << pos.en_passant;
                blocker_mask |= (king_pos << 7);
            }
            if ((king_pos << 9) & pos.b_pawns) {
                if (pos.en_passant > 0 && (1ull << pos.en_passant) & (king_pos << 17))
                    blocker_mask |= 1ull << pos.en_passant;
                blocker_mask |= (king_pos << 9);
            }
        }
        // If the king is on the same rank as en passant pawn it is possible have a special en passant pin
        if (pos.en_passant > 0) {
            bitboard king_rank = get_hline(king_pos);
            bitboard en_passant_pawn = 1ull << (pos.en_passant - 8);
            if (king_rank & en_passant_pawn) {
                if (pos.b_pieces & (en_passant_pawn << 1))
                {
                    bitboard wo_left = pos.all_pieces & !(en_passant_pawn | (en_passant_pawn << 1));
                    bitboard pinner_mask = movgen::generate_piece_moves<movgen::Piece::ROOK>(king_pos, wo_left, hashed_moves) & king_rank;
                    if (pinner_mask & (pos.w_queens | pos.w_rooks))
                        pos.en_passant = 0;
                }
                if (pos.b_pieces & (en_passant_pawn >> 1))
                {
                    bitboard wo_left = pos.all_pieces & !(en_passant_pawn | (en_passant_pawn >> 1));
                    bitboard pinner_mask = movgen::generate_piece_moves<movgen::Piece::ROOK>(king_pos, wo_left, hashed_moves) & king_rank;
                    if (pinner_mask & (pos.w_queens | pos.w_rooks))
                        pos.en_passant = 0;
                }
            }
        }
    }

    // In case of double check only a king can move
    if (attackers >= 2) {
        if (static_cast<unsigned int>(pos.side))
        {
            bitboard king_moves = movgen::generate_piece_moves(king_pos, pos.all_pieces, hashed_moves) &
                ~(queen_attacks | rook_attacks | bishop_attacks | knight_attacks | pawn_attacks | king_attacks) & ~pos.w_pieces;

            for (auto square : movgen::bitscan(king_moves))
                legal_moves.legal_moves.push_back(movgen::Move(movgen::Piece::W_KING, king_pos, square, movgen::get_piece(square, 2)));
        }
        else
        {
            bitboard king_moves = movgen::generate_piece_moves(king_pos, pos.all_pieces, hashed_moves) &
                ~(queen_attacks | rook_attacks | bishop_attacks | knight_attacks | pawn_attacks | king_attacks) & ~pos.b_pieces;

            for (auto square : movgen::bitscan(king_moves))
                legal_moves.legal_moves.push_back(movgen::Move(movgen::Piece::W_KING, king_pos, square, movgen::get_piece(square, 1)));
        }
        return legal_moves;
    }
    else if (attackers == 1) {
        // Apply a blocker mask to all moves

        //Pinned pieces cannot move, remove them from the board
        bitboard king_sliding = movgen::generate_piece_moves<movgen::Piece::BISHOP>(king_pos, pos.all_pieces, hashed_moves);

        if (static_cast<unsigned int>(pos.side))
        {
            bitboard pinned_pieces = king_sliding & pos.w_pieces;
            bitboard pinners = movgen::generate_piece_moves<movgen::Piece::BISHOP>(king_pos,
                pos.all_pieces ^ pinned_pieces, hashed_moves) ;

        }
        else
        {

        }
    }

    // Generate moves for pinned pieces and remove them from the board
    bitboard king_sliding = movgen::generate_piece_moves<movgen::Piece::QUEEN>(king_pos, pos.all_pieces, hashed_moves);
    // Number of checks is zero, so these attacks can only intersect on friendly pieces
    bitboard pinned = king_sliding & (bishop_attacks | queen_attacks | rook_attacks);

    for (bpos piece_pos : movgen::bitscan(pinned)) {
        bitboard pinned_mask;
        if (piece_pos % 8 > king_pos % 8)
            pinned_mask = movgen::get_rdiag(piece_pos);
        else
            pinned_mask = movgen::get_ldiag(piece_pos);

        movgen::Piece cur_piece = movgen::get_piece(pos, piece_pos, static_cast<unsigned int>(pos.side) + 1);

        bitboard piece_moves = movgen::generate_piece_moves<cur_piece>();
    }

    return legal_moves;
}

void movgen::generateKingMoves(movgen::BoardPosition& pos,
    movgen::GeneratedMoves* moves,
    movgen::MoveCache* cache) {
    moves->w_king_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_kings))
        moves->w_king_attacks |= cache->king_cache[sq];
    moves->w_attacks |= moves->w_king_attacks;

    moves->b_king_attacks = 0;
    for (auto sq : movgen::bitscan(pos.b_kings))
        moves->b_king_attacks |= cache->king_cache[sq];
    moves->b_attacks |= moves->b_king_attacks;

    // This has to be generated two times, because we also should consider king
    // attacks for each move
    for (auto sq : movgen::bitscan(pos.w_kings)) {
        bitboard cur_moves = cache->knight_cache[sq];
        cur_moves &= !(pos.w_pieces | moves->b_attacks);
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::W_KING, sq, move, (int)get_piece(pos, move, 2)));
    }
    for (auto sq : movgen::bitscan(pos.b_kings)) {
        bitboard cur_moves = cache->knight_cache[sq];
        cur_moves &= !(pos.b_pieces | moves->w_attacks);
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::B_KING, sq, move, (int)get_piece(pos, move, 1)));
    }
}

void movgen::generateQueenMoves(movgen::BoardPosition& pos,
    movgen::GeneratedMoves* moves,
    movgen::GeneratedMagics* magics) {
    moves->w_queen_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_queens)) {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics) |
            movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->w_queen_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::W_QUEEN, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_queen_attacks;

    moves->b_queen_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_queens)) {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics) |
            movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_queen_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::B_QUEEN, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_queen_attacks;
}

void movgen::generateRookMoves(movgen::BoardPosition& pos,
    movgen::GeneratedMoves* moves,
    movgen::GeneratedMagics* magics) {
    moves->w_rook_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_rooks)) {
        bitboard cur_moves = movgen::get_rook_attacks(pos.all_pieces, sq, magics);
        moves->w_rook_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::W_ROOK, sq, move, (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_rook_attacks;

    moves->b_rook_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_rooks)) {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_rook_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(
                movgen::Move(Piece::B_ROOK, sq, move, (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_rook_attacks;
}

void movgen::generateBishopMoves(movgen::BoardPosition& pos,
    movgen::GeneratedMoves* moves,
    movgen::GeneratedMagics* magics) {
    moves->w_bishop_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_bishops)) {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->w_bishop_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_BISHOP, sq, move,
                (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_bishop_attacks;

    moves->b_bishop_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_bishops)) {
        bitboard cur_moves = movgen::get_bishop_attacks(pos.all_pieces, sq, magics);
        moves->b_bishop_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_BISHOP, sq, move,
                (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_bishop_attacks;
}

void movgen::generateKnightMoves(movgen::BoardPosition& pos,
    movgen::GeneratedMoves* moves,
    movgen::MoveCache* cache) {
    moves->w_knight_attacks = 0;
    for (auto sq : movgen::bitscan(pos.w_knights)) {
        bitboard cur_moves = cache->knight_cache[sq];
        moves->w_knight_attacks |= cur_moves;

        cur_moves &= !pos.w_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::W_KNIGHT, sq, move,
                (int)get_piece(pos, move, 2)));
    }
    moves->w_attacks |= moves->w_knight_attacks;

    moves->b_knight_attacks = 0;
    for (auto sq : movgen::bitscan(pos.b_knights)) {
        bitboard cur_moves = cache->knight_cache[sq];
        moves->b_knight_attacks |= cur_moves;

        cur_moves &= !pos.b_pieces;
        for (auto move : movgen::bitscan(cur_moves))
            moves->moves.push_back(movgen::Move(Piece::B_KNIGHT, sq, move,
                (int)get_piece(pos, move, 1)));
    }
    moves->b_attacks |= moves->b_knight_attacks;
}

void movgen::generatePawnMoves(movgen::BoardPosition& board,
    movgen::GeneratedMoves* moves) {
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
    left_pawn_captures =
        left_pawn_attacks &
        (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != 0));
    right_pawn_captures =
        right_pawn_attacks &
        (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != 0));

    for (auto move : movgen::bitscan(left_pawn_captures & ~top_edge))
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 9, move,
            (int)get_piece(board, move, 2)));
    for (auto move : movgen::bitscan(right_pawn_captures& top_edge))
        moves->moves.push_back(movgen::Move(Piece::W_PAWN, move - 7, move,
            (int)get_piece(board, move, 2), 5));

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
    left_pawn_captures =
        left_pawn_attacks &
        (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != 1));
    right_pawn_captures =
        right_pawn_attacks &
        (board.b_pieces | (1ull << board.en_passant) * (board.en_passant != 1));

    for (auto move : movgen::bitscan(left_pawn_captures & ~bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 7, move,
            (int)get_piece(board, move, 1)));
    for (auto move : movgen::bitscan(right_pawn_captures& bottom_edge))
        moves->moves.push_back(movgen::Move(Piece::B_PAWN, move + 9, move,
            (int)get_piece(board, move, 1), 5));
}
