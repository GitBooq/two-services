#include <chrono>
#include <cstdint>

#include "net_logger/types.h"

namespace builder_utils {

using TimePoint = std::chrono::system_clock::time_point;
std::string FormatTimestamp(const TimePoint &timestamp);

std::optional<std::string>
FilterRejectReasonToStr(net::logger::RejectReason reason);

std::string GetFilterDecision(net::logger::RejectReason reason);

std::string IPv4AddressToString(uint32_t addr);

} // namespace builder_utils