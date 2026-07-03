#include "event_builder.h"
#include "details/ip_v4_address.h"

#include <chrono>

using TimePoint = std::chrono::system_clock::time_point;
std::string formatTimestamp(const TimePoint &timestamp);
std::optional<std::string>
filterRejectReasonToStr(net::logger::RejectReason reason);
std::string getFilterDecision(net::logger::RejectReason reason);
std::string IPv4AddressToString(uint32_t addr);

dto::Event EventBuilder::Build(const net::logger::LogEntry &log_entry) {
  dto::Event event;
  event.source_service = src_service_name_;
  event.timestamp_utc = formatTimestamp(std::chrono::system_clock::now());
  event.status = log_entry.parsed_ip.has_value() ? dto::Event::Status::SUCCESS
                                                 : dto::Event::Status::ERROR;

  dto::Payload payload;
  payload.raw_line = log_entry.raw_line;
  payload.parsed_ip = IPv4AddressToString(log_entry.parsed_ip->ToUint32());
  payload.filter_decision = getFilterDecision(log_entry.reason);
  payload.reject_reason = filterRejectReasonToStr(log_entry.reason);

  event.payload = std::move(payload);

  return event;
}

std::string formatTimestamp(const TimePoint &timestamp) {
  auto time_t = std::chrono::system_clock::to_time_t(timestamp);
  auto *tm = std::localtime(&time_t);

  std::ostringstream oss;
  oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S"); // 2026-05-10 15:30:01
  return oss.str();
}

std::optional<std::string>
filterRejectReasonToStr(net::logger::RejectReason reason) {
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

std::string getFilterDecision(net::logger::RejectReason reason) {
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