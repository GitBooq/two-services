#include "utils.h"

namespace builder_utils {

std::string FormatTimestamp(const TimePoint &timestamp) {
  auto time_t = std::chrono::system_clock::to_time_t(timestamp);
  auto *tm = std::localtime(&time_t);

  std::ostringstream oss;
  oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S"); // 2026-05-10 15:30:01
  return oss.str();
}

std::optional<std::string>
FilterRejectReasonToStr(net::logger::RejectReason reason) {
  switch (reason) {
  case net::logger::RejectReason::InvalidFormat:
    return "Invalid Format";
  case net::logger::RejectReason::InvalidIPAddress:
    return "InvalidIPAddress";
  case net::logger::RejectReason::FilterRejected:
    return "FilterRejected";
  case net::logger::RejectReason::None:
  default:
    return std::nullopt;
  }
}

std::string GetFilterDecision(net::logger::RejectReason reason) {
  switch (reason) {
  case net::logger::RejectReason::FilterRejected:
    return "rejected";
  case net::logger::RejectReason::None:
    return "accepted";
  default:
    return "";
  }
}

// Convert from network byte order addr
// to string representation (xxx.xxx.xxx.xxx)
std::string IPv4AddressToString(uint32_t addr) {
  uint8_t octet1 = (addr >> 24) & 0xFF;
  uint8_t octet2 = (addr >> 16) & 0xFF;
  uint8_t octet3 = (addr >> 8) & 0xFF;
  uint8_t octet4 = addr & 0xFF;

  return std::to_string(octet1) + "." + std::to_string(octet2) + "." +
         std::to_string(octet3) + "." + std::to_string(octet4);
}

} // namespace builder_utils