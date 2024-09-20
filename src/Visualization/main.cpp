#include <SFML/Graphics.hpp>

#include "MagicNumbers.h"
#include "Bitboard.h"
#include "MoveGeneration.h"

#include "headers/game.h"
#include <iostream>
#include <thread>
#include <string>

/// @brief Catch and process all thrown exceptions, that bubble past main
void terminate_func();

int main()
{
    std::thread init_thread1(movgen::init);
    std::thread init_thread2(bitb::init);

    init_thread1.detach();
    init_thread2.detach();

    try
    {
        //Chess chess({ 1000, 1000 });
        Chess chess({1000, 1000}, "rnbB1k1r/pp1P1ppp/2pb4/8/2B5/8/PPP1NnPP/RN1QK2R b KQ - 0 9");
        chess.loop();
    }
    catch (std::exception ex)
    {
        terminate_func();
    }

    return 0;
}

void terminate_func()
{
    std::cerr << "terminate called after throwing an instance of ";
    try
    {
        std::rethrow_exception(std::current_exception());
    }
    catch (const std::exception &ex)
    {
        std::cerr << typeid(ex).name() << std::endl;
        std::cerr << "  what(): " << ex.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << typeid(std::current_exception()).name() << std::endl;
        std::cerr << " ...something, not an exception, dunno what." << std::endl;
    }
    std::cerr << "errno: " << errno << ": " << std::strerror(errno) << std::endl;
    std::abort();
}
