#include "core/log_level.h"
#include "core/logger.h"
#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>

TEST_CASE("Logger_Should_WriteLogToRotatingFile")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-logger-tests";
    std::filesystem::remove_all(log_directory);

    im_cpp::core::Logger logger("im-cpp-test");
    logger.setup(im_cpp::core::LogLevel::debug, log_directory);
    const auto guiSink = logger.attachGuiSink();
    SPDLOG_LOGGER_INFO(logger.get(), "test log message");
    logger.shutdown();

    std::ifstream log_file(log_directory / "im-cpp-test.log");
    const std::string contents((std::istreambuf_iterator<char>(log_file)), std::istreambuf_iterator<char>());
    const auto guiEntries = guiSink->drain();
    REQUIRE(contents.contains("test log message"));
    REQUIRE(guiEntries.size() == 1);
#ifdef IM_CPP_DEBUG_BUILD
    constexpr std::string_view expectedLogSuffix{" [info] [logger_tests.cpp] [20] test log message\n"};
#else
    constexpr std::string_view expectedLogSuffix{" [info] test log message\n"};
#endif
    REQUIRE(guiEntries.front().formattedMessage.ends_with(expectedLogSuffix));
    REQUIRE(contents == guiEntries.front().formattedMessage);

    std::filesystem::remove_all(log_directory);
}

TEST_CASE("Logger_Should_UseSourceFilenameForDebugAndTraceAndDisableThemInRelease")
{
    const auto log_directory = std::filesystem::temp_directory_path() / "im-cpp-logger-level-tests";
    std::filesystem::remove_all(log_directory);

    im_cpp::core::Logger logger("im-cpp-level-test");
    logger.setup(im_cpp::core::LogLevel::trace, log_directory);
    const auto guiSink = logger.attachGuiSink();
    logger.setLevel(im_cpp::core::LogLevel::trace);
    const auto activeLogger = logger.get();
    constexpr int debugSourceLine = 42;
    constexpr int traceSourceLine = 43;
    activeLogger->log(
        spdlog::source_loc{"src/tests/debug_source.cpp", debugSourceLine, ""}, spdlog::level::debug, "debug message");
    activeLogger->log(
        spdlog::source_loc{"src/tests/trace_source.cpp", traceSourceLine, ""}, spdlog::level::trace, "trace message");
    logger.shutdown();

    const auto entries = guiSink->drain();
#ifdef RELEASE
    REQUIRE(entries.empty());
#else
    REQUIRE(entries.size() == 2);
#ifdef IM_CPP_DEBUG_BUILD
    REQUIRE(entries.at(0).formattedMessage.ends_with(" [debug] [debug_source.cpp] [42] debug message\n"));
    REQUIRE(entries.at(1).formattedMessage.ends_with(" [trace] [trace_source.cpp] [43] trace message\n"));
#else
    REQUIRE(entries.at(0).formattedMessage.ends_with(" [debug] [debug_source.cpp] debug message\n"));
    REQUIRE(entries.at(1).formattedMessage.ends_with(" [trace] [trace_source.cpp] trace message\n"));
#endif
#endif

    std::filesystem::remove_all(log_directory);
}
