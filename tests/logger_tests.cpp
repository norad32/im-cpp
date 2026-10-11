#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <string>
#include "core/log_level.h"
#include "core/logger.h"

TEST_CASE("Logger_Should_WriteLogToRotatingFile")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-logger-tests";
    std::filesystem::remove_all(log_directory);

    im_cpp::core::Logger logger("im-cpp-test");
    logger.setup(im_cpp::core::LogLevel::debug, log_directory);
    logger.get()->info("test log message");
    logger.shutdown();

    std::ifstream log_file(log_directory / "im-cpp-test.log");
    const std::string contents((std::istreambuf_iterator<char>(log_file)), std::istreambuf_iterator<char>());
    REQUIRE(contents.contains("test log message"));

    std::filesystem::remove_all(log_directory);
}
