#include <cstddef>
#include <cstdint>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "headers/commands.h"

constexpr uint16_t TABLE_SIZE = 3;


// Struct that is used to call functions from cmd
struct CommandStruct
{
    const char* command;
    void (*commandHandler)(std::vector<std::string> args);
} commandTable[] = {
    { "position", save_position },
    { "search", start_search },
    { "perft", start_perft }
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

    return ret_vec;
}

int main() {
    while(true)
    {
        static std::string line;

        std::cin >> line;
        std::vector<std::string> split_line = split_string(line, " ");

        // Firs argument is commend name, the rest are cmd argument for a function
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
        printf("\"%s\" was not found", command.c_str());
    LoopEnd:
        continue;
	}

    return 0;
}
