#pragma once

#include <mutex>
#include <string>
#include <vector>
#include <spdlog/details/log_msg.h>
#include <spdlog/sinks/base_sink.h>

namespace im_cpp::core
{

inline constexpr std::size_t defaultGuiLogCapacity = 1024;

struct GuiLogEntry
{
    spdlog::level::level_enum level;
    std::string message;
};

class GuiLogSink final : public spdlog::sinks::base_sink<std::mutex>
{
public:
    explicit GuiLogSink(std::size_t capacity = defaultGuiLogCapacity);

    [[nodiscard]] std::vector<GuiLogEntry> drain();
    void clear();

protected:
    void sink_it_(const spdlog::details::log_msg & message) override;
    void flush_() override;

private:
    const std::size_t capacity_;
    std::vector<GuiLogEntry> entries_;
};

} // namespace im_cpp::core
