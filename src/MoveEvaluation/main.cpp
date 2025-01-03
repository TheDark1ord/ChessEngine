#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <thread>

#include "headers/commands.h"
#include "MoveGeneration.h"

constexpr uint16_t TABLE_SIZE = 4;

void print_help(std::vector<std::string> args);

// Struct that is used to call functions from cmd
struct CommandStruct
{
    const char* command;
    void (*commandHandler)(std::vector<std::string> args);
} commandTable[TABLE_SIZE] = {
    { "position", save_position },
    { "search", start_search },
    { "perft", start_perft },
    { "help", print_help }
};

std::vector<std::string> split_string(std::string str, std::string delim)
{
    std::vector<std::string> ret_vec;

    size_t pos = 0;
    while((pos = str.find(delim)) != std::string::npos)
    {
        ret_vec.push_back(str.substr(0, pos));
        str.erase(0, pos + delim.length());
    }
    ret_vec.push_back(str);

    return ret_vec;
}

int main() {
    std::thread init_thread1(movgen::init);
	std::thread init_thread2(bitb::init);

    srand(time(NULL));

    printf("Initializing...\n");
	init_thread1.detach();
	init_thread2.join();
    printf("Engine is ready, input a command\n");

    while(true)
    {
        static std::string line;

        printf(">");
        std::getline(std::cin, line);
        std::vector<std::string> split_line = split_string(line, " ");

        // Firs argument is command name, the rest are cmd arguments for a function
        std::string command = split_line[0];
        auto args = std::vector<std::string>(split_line.begin() + 1, split_line.end());

        // Iterate through command aliases and call the corresponding function
        for(int i = 0; i < TABLE_SIZE; i++)
        {
            if(command == commandTable[i].command)
            {
                try {
                    commandTable[i].commandHandler(args);
                } catch (std::exception e) {
                    std::cout << e.what() << std::endl;
                }
                // Break out of the loop and skip not found statement
                goto LoopEnd;
            }
        }
        // Command was not found (did not break from the loop)
        printf("Command \"%s\" was not found\n", command.c_str());
    LoopEnd:
        continue;
    }

    return 0;
}

void print_help(std::vector<std::string> args)
{
    if (args.size() == 0)
    {
        printf("Get a grip! You are a man! You don't need any help!\n");
        return;
    }
    printf("Get a grip! You are a man! You don't need any help!\n");
}