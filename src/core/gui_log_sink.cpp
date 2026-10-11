#include "core/gui_log_sink.h"
#include <cstddef>
#include <mutex>
#include <utility>
#include <vector>
#include <fmt/format.h>
#include <spdlog/details/log_msg.h>

namespace im_cpp::core
{

GuiLogSink::GuiLogSink(std::size_t capacity) : capacity_(capacity)
{
    entries_.reserve(capacity);
}

std::vector<GuiLogEntry> GuiLogSink::drain()
{
    const std::scoped_lock lock(mutex_);
    std::vector<GuiLogEntry> entries;
    entries.swap(entries_);
    return entries;
}

void GuiLogSink::clear()
{
    const std::scoped_lock lock(mutex_);
    entries_.clear();
}

void GuiLogSink::sink_it_(const spdlog::details::log_msg & message)
{
    GuiLogEntry entry{.level = message.level, .message = fmt::to_string(message.payload)};
    if (capacity_ == 0)
    {
        return;
    }

    if (entries_.size() == capacity_)
    {
        entries_.erase(entries_.begin());
    }
    entries_.push_back(std::move(entry));
}

void GuiLogSink::flush_()
{
}

} // namespace im_cpp::core
