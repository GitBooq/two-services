#pragma once

#include "details/filter.h"        // for FilterType
#include "details/ip_v4_address.h" // for IPv4Address
#include "core/log_entry.h"

#include <string>
#include <vector>

namespace core::logProcessor {

enum class LogStatus { ACCEPTED, REJECTED };

using net::details::IPv4Address;

inline std::optional<IPv4Address> ParseIPFromLine(const std::string &line) {
  size_t end = line.find(' ');
  if (end == std::string::npos) {
    return std::nullopt;
  }

  std::string_view ip_str(line.data(), end);
  return IPv4Address::FromString(ip_str);
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

class LogProcessor {
public:
  LogProcessor() = default;

  template <net::details::FilterType T>
  void Process(std::istream &input, const T &filter);

  std::vector<LogEntry> entries() const { return entries_; }

private:
  std::vector<LogEntry> entries_;
};

template <net::details::FilterType T>
void LogProcessor::Process(std::istream &input, const T &filter) {
  std::string line;
  while (std::getline(input, line)) {
    auto ip = ParseIPFromLine(line);
    if (!ip.has_value()) {
      continue;
    }

    LogEntry parsed{.raw_line = line,
                    .parsed_ip = IPv4AddressToString(ip->ToUint32()),
                    .filter_decision = filter.Matches(*ip)
                                           ? LogStatus::ACCEPTED
                                           : LogStatus::REJECTED};

    entries_.push_back(parsed);
  }
}

} // namespace core::logProcessor