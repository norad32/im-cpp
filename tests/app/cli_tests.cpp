#include "app/cli.h"
#include "core/logger.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <memory>
#include <spdlog/common.h>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

TEST_CASE("runCli_Should_PrintCheckResult")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-cli-check-tests";
    std::filesystem::remove_all(log_directory);
    const std::vector<std::string_view> arguments{"check", "--log-level", "DEBUG"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-test");

    const int result = im_cpp::app::runCli(arguments, output, error, logger, log_directory);

    REQUIRE(result == 0);
    REQUIRE(output.str() == "I'm C++ OK\n");
    REQUIRE(error.str().empty());
    std::ifstream logFile(log_directory / "im-cpp-cli-test.log");
    const std::string logContents((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    REQUIRE(logContents.contains("Application started"));
    REQUIRE(logContents.contains("Application closed"));
    std::filesystem::remove_all(log_directory);
}

TEST_CASE("runCli_Should_PrintVersion")
{
    const std::vector<std::string_view> arguments{"--version"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-version-test");

    const int result = im_cpp::app::runCli(arguments, output, error, logger);

    REQUIRE(result == 0);
    REQUIRE(output.str() == "im-cpp 0.1.0\n");
    REQUIRE(error.str().empty());
}

TEST_CASE("runCli_Should_InvokeGuiRunnerWhenGuiCommandIsSelected")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-cli-gui-tests";
    std::filesystem::remove_all(log_directory);
    const std::vector<std::string_view> arguments{"gui"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-gui-test");
    bool gui_was_run = false;
    spdlog::level::level_enum gui_log_level = spdlog::level::off;

    const int result = im_cpp::app::runCli(arguments,
                                           output,
                                           error,
                                           logger,
                                           log_directory,
                                           [&gui_was_run, &gui_log_level](std::shared_ptr<spdlog::logger> active_logger,
                                                                          std::shared_ptr<im_cpp::core::GuiLogSink> log_sink)
                                           {
                                               gui_was_run = active_logger != nullptr && log_sink != nullptr;
                                               gui_log_level = active_logger->level();
                                               return 7;
                                           });

    REQUIRE(result == 7);
    REQUIRE(gui_was_run);
#ifdef RELEASE
    REQUIRE(gui_log_level == spdlog::level::info);
#else
    REQUIRE(gui_log_level == spdlog::level::debug);
#endif
    REQUIRE(error.str().empty());
    std::ifstream logFile(log_directory / "im-cpp-cli-gui-test.log");
    const std::string logContents((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    REQUIRE(logContents.contains("Application started"));
    REQUIRE(logContents.contains("Application closed"));
    std::filesystem::remove_all(log_directory);
}

TEST_CASE("runCli_Should_RespectExplicitGuiLogLevel")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-cli-gui-log-level-tests";
    std::filesystem::remove_all(log_directory);
    const std::vector<std::string_view> arguments{"gui", "--log-level", "INFO"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-gui-log-level-test");
    spdlog::level::level_enum gui_log_level = spdlog::level::off;

    const int result = im_cpp::app::runCli(
        arguments,
        output,
        error,
        logger,
        log_directory,
        [&gui_log_level](std::shared_ptr<spdlog::logger> active_logger, std::shared_ptr<im_cpp::core::GuiLogSink>)
        {
            gui_log_level = active_logger->level();
            return 0;
        });

    REQUIRE(result == 0);
    REQUIRE(gui_log_level == spdlog::level::info);
    REQUIRE(error.str().empty());
    std::filesystem::remove_all(log_directory);
}

TEST_CASE("runCli_Should_LogGuiRunnerException")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-cli-gui-exception-tests";
    std::filesystem::remove_all(log_directory);
    const std::vector<std::string_view> arguments{"gui"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-gui-exception-test");

    const int result = im_cpp::app::runCli(arguments,
                                           output,
                                           error,
                                           logger,
                                           log_directory,
                                           [](std::shared_ptr<spdlog::logger>, std::shared_ptr<im_cpp::core::GuiLogSink>) -> int
                                           { throw std::runtime_error("GUI failure"); });

    std::ifstream logFile(log_directory / "im-cpp-cli-gui-exception-test.log");
    const std::string logContents((std::istreambuf_iterator<char>(logFile)), std::istreambuf_iterator<char>());
    REQUIRE(result == 1);
    REQUIRE(logContents.contains("[error] [im-cpp-cli-gui-exception-test]"));
    REQUIRE(logContents.contains("Application failed: GUI failure"));
    REQUIRE(error.str() == "Application failed: GUI failure\n");
    std::filesystem::remove_all(log_directory);
}

TEST_CASE("runCli_Should_ReportLoggerSetupFailure")
{
    const auto log_path = std::filesystem::temp_directory_path() / "im-cpp-cli-logger-setup-failure";
    const auto blocker_path = log_path / "file-blocker";
    std::filesystem::remove_all(log_path);
    std::filesystem::create_directories(log_path);
    std::ofstream(blocker_path).put('x');
    const std::vector<std::string_view> arguments{"check"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-logger-setup-failure-test");

    const int result = im_cpp::app::runCli(arguments, output, error, logger, blocker_path);

    REQUIRE(result == 1);
    REQUIRE(output.str().empty());
    REQUIRE(error.str().starts_with("Logger setup failed: "));
    std::filesystem::remove_all(log_path);
}

TEST_CASE("runCli_Should_RejectInvalidLogLevel")
{
    const std::vector<std::string_view> arguments{"check", "--log-level", "35"};
    std::ostringstream output;
    std::ostringstream error;
    im_cpp::core::Logger logger("im-cpp-cli-invalid-level-test");

    const int result = im_cpp::app::runCli(arguments, output, error, logger);

    REQUIRE(result == 2);
    REQUIRE(error.str() == "Invalid log level: 35\n");
}
