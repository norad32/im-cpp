#include "app/cli.h"
#include "core/gui_log_sink.h"
#include "core/logger.h"
#include "gui/application.h"
#include <cstddef>
#include <iostream>
#include <memory>
#include <span>
#include <spdlog/logger.h>
#include <string_view>
#include <utility>
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
    return im_cpp::app::runCli(
        arguments,
        std::cout,
        std::cerr,
        logger,
        {},
        [](std::shared_ptr<spdlog::logger> activeLogger, std::shared_ptr<im_cpp::core::GuiLogSink> logSink)
        {
            im_cpp::gui::Application application(std::move(activeLogger), std::move(logSink));
            return application.run();
        });
}
