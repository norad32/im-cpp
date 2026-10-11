#include <cstddef>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>
#include "app/cli.h"
#include "core/logger.h"
#if IM_CPP_HAS_GUI
#include <memory>
#include <utility>
#include <spdlog/logger.h>
#include "core/gui_log_sink.h"
#include "gui/application.h"
#endif

int main(int argc, char * argv[])
{
    std::vector<std::string_view> arguments;
    arguments.reserve(static_cast<std::size_t>(argc > 0 ? argc - 1 : 0));
    const std::span<char *> argumentArray(argv, static_cast<std::size_t>(argc));
    for (const char * argument : argumentArray.subspan(1))
    {
        arguments.emplace_back(argument);
    }

    im_cpp::core::Logger logger;
#if IM_CPP_HAS_GUI
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
#else
    return im_cpp::app::runCli(arguments, std::cout, std::cerr, logger);
#endif
}
