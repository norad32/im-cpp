#include "app/cli.h"
#include "core/logger.h"
#include <CLI/CLI.hpp>
#include <cstddef>
#include <iostream>
#include <memory>
#include <span>
#include <string_view>
#include <vector>

int main(int argc, char * argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    for (int index = 1; index < argc; ++index)
    {
        arguments.emplace_back(argv[index]);
    }
    im_cpp::core::Logger logger;
    return im_cpp::app::runCli(arguments, std::cout, std::cerr, logger);
}
