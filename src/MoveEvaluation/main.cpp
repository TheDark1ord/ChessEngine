#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "headers/commands.h"

constexpr uint16_t TABLE_SIZE = 3;

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
        auto split_line = split_string(line, " ");
        auto command = split_line[0];
        auto args = std::vector<std::string>(split_line.begin() + 1, split_line.end());

		for(int i = 0; i < TABLE_SIZE; i++)
		{
			if(command == commandTable[i].command)
			{
				commandTable[i].commandHandler(args);
				break;
			}
		}
	}

    return 0;
}
