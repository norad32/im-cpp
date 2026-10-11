#include <catch2/catch_test_macros.hpp>
#include <memory>
#include <spdlog/logger.h>
#include "core/gui_log_sink.h"

TEST_CASE("GuiLogSink_Should_KeepNewestEntriesWhenCapacityIsReached")
{
    auto sink = std::make_shared<im_cpp::core::GuiLogSink>(2);
    spdlog::logger logger("gui-sink-test", sink);

    logger.info("first");
    logger.warn("second");
    logger.error("third");

    const auto entries = sink->drain();
    REQUIRE(entries.size() == 2);
    REQUIRE(entries[0].message == "second");
    REQUIRE(entries[1].message == "third");
    REQUIRE(sink->drain().empty());
}

TEST_CASE("GuiLogSink_Should_ClearBufferedEntries")
{
    auto sink = std::make_shared<im_cpp::core::GuiLogSink>();
    spdlog::logger logger("gui-sink-clear-test", sink);

    logger.info("entry");
    sink->clear();

    REQUIRE(sink->drain().empty());
}
