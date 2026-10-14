#include "app/cli.h"
#include "core/logger.h"
#include <cstddef>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

int main(int argc, char * argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    if (argc > 1)
    {
        for (const char * argument : std::span(argv, static_cast<std::size_t>(argc)).subspan(1))
        {
            arguments.emplace_back(argument);
        }
    }
    im_cpp::core::Logger logger;
    return im_cpp::app::runCli(arguments, std::cout, std::cerr, logger);
}
