#include "MoveGeneration.h"
#include "MagicNumbers.h"
#include "MovgenTypes.h"

#include <type_traits>
#include <thread>
#include <stack>
#include <cstdint>
#include <cstdlib>
#include <random>
#include <iostream>

const std::string fen_strings[]{
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",         // Initial position
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq -", // Kiwipete by Peter McKenzie
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - -",                            // pos 3
    "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", // pos 4
    "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1", // Mirrored pos 4
    "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",        // pos 5
};
const std::vector<uint64_t> positions_reached[]{
    {20, 400, 8902, 197281, 4865609, 119060324, 3195901860, 84998978956},
    {48, 2039, 97862, 4085603, 193690690, 8031647685},
    {14, 191, 2812, 43238, 674624, 11030083, 178633661, 3009794393},
    {6, 264, 9467, 422333, 15833292, 706045033},
    {6, 264, 9467, 422333, 15833292, 706045033},
    {44, 1486, 62379, 2103487, 89941194},
};
const char *squares[]{
    "h1", "g1", "f1", "e1", "d1", "c1", "b1", "a1",
    "h2", "g2", "f2", "e2", "d2", "c2", "b2", "a2",
    "h3", "g3", "f3", "e3", "d3", "c3", "b3", "a3",
    "h4", "g4", "f4", "e4", "d4", "c4", "b4", "a4",
    "h5", "g5", "f5", "e5", "d5", "c5", "b5", "a5",
    "h6", "g6", "f6", "e6", "d6", "c6", "b6", "a6",
    "h7", "g7", "f7", "e7", "d7", "c7", "b7", "a7",
    "h8", "g8", "f8", "e8", "d8", "c8", "b8", "a8",
};

uint64_t count_moves(movgen::BoardPosition &initial, std::vector<movgen::Move> *cur_moves, unsigned int depth, bool toplevel = false);

int main(int argc, char *argv[])
{
    std::thread init_thread1(movgen::init);
    std::thread init_thread2(bitb::init);

    init_thread1.detach();
    init_thread2.join();

    uint16_t test_index = std::atoi(argv[1]);
    const std::string *fen_string = &fen_strings[test_index];
    const std::vector<uint64_t> *pos_num = &positions_reached[test_index];

    movgen::BoardPosition initial_position = movgen::board_from_fen(*fen_string);
    //movgen::BoardPosition initial_position = movgen::board_from_fen("r2k3r/p1ppqNb1/bn2pQp1/3P4/1p2P3/2N4p/PPPBBPPP/R3K2R b KQ - 0 2");

    int depth = pos_num->size();
    if (argc > 2)
        depth = std::atoi(argv[2]);

    std::vector<movgen::Move> *cur_moves = initial_position.side_to_move == movgen::WHITE ?
                movgen::generate_all_moves<movgen::WHITE>(initial_position) :
                movgen::generate_all_moves<movgen::BLACK>(initial_position);
    cur_moves = movgen::get_legal_moves(initial_position, *cur_moves);

    int initial_depth = 0;
    for (int i = 3; i < argc; i++)
    {
        std::vector<movgen::Move>* new_moves = cur_moves;
        for (auto& move : *new_moves)
        {
            if ((std::string(squares[move.from]) + (std::string(squares[move.to]))) == argv[i])
            {
                movgen::make_move(&initial_position, move, &new_moves);
                break;
            }
        }
        cur_moves = new_moves;

        depth--;
        initial_depth = depth - 1;
    }

    for (int i = initial_depth; i < depth; i++)
    {
        printf("Depth: %d\n", i + 1);

        if (depth == 1)
            for (auto& move : *cur_moves)
                printf("%s%s: %u\n", squares[move.from], squares[move.to], 1);

        uint64_t counted = count_moves(initial_position, cur_moves, i + 1, true);

        printf("\nNodes reached: %llu\n\n", counted);

        if (argc < 4 && counted != (*pos_num)[i])
            return -1;
    }

    return 0;
}

uint64_t count_moves(movgen::BoardPosition &initial, std::vector<movgen::Move> *cur_moves, unsigned int depth, bool toplevel)
{
    if (depth == 1)
        return cur_moves->size();

    std::vector<movgen::Move>* new_moves = nullptr;

    uint64_t move_count = 0;
    for (auto& move : *cur_moves)
    {
        auto result = movgen::make_move(&initial, move, &new_moves);

        if (result == movgen::GAME_CONTINUES)
        {
            auto counted = count_moves(initial, new_moves, depth - 1);
            move_count += counted;

            if (toplevel)
                printf("%s%s: %llu\n", squares[move.from], squares[move.to], counted);
        }
        movgen::undo_move(&initial, move);
    }
    
    if (new_moves != nullptr)
    {
        std::vector<movgen::Move>().swap(*new_moves);
        delete new_moves;
        new_moves = nullptr;
    }
    return move_count;
}